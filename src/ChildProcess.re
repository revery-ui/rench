include ChildProcessTypes;

type innerProcess = {
  pid: int,
  stdout: outputPipe,
  stdin: inputPipe,
  stderr: outputPipe,
  onClose: Event.t(int),
  exitCode: ref(option(int)),
  kill: int => unit,
  _readThread: Thread.t,
  _waitThread: Thread.t,
  _errThread: Thread.t,
};

let _formatEnvironmentVariables = (env: EnvironmentVariables.t) => {
  let f = (key, value, prev) => {
    Array.append(prev, [|key ++ "=" ++ value|]);
  };

  EnvironmentVariables.fold(~f, env, [||]);
};

let _safeClose = v => {
  switch (Unix.close(v)) {
  /* We may get a BADF if this is called too soon after opening a process */
  | exception (Unix.Unix_error(_)) => ()
  | _ => ()
  };
};

let _withWorkingDirectory = (wd: option(string), f) => {
  let currentDirectory = Sys.getcwd();

  switch (wd) {
  | None => ()
  | Some(x) => Sys.chdir(x)
  };

  let ret = f();

  switch (wd) {
  | None => ()
  | Some(_) => Sys.chdir(currentDirectory)
  };

  ret;
};

let createReadingThread = (pipe, pipe_onData, isRunning) =>
  Thread.create(
    ((pipe, pipe_onData)) => {
      let buffer = Buffer.create(8192);
      let bytes = Bytes.create(8192);

      let isReading = ref(true);

      let flush = () => {
        let out = Buffer.to_bytes(buffer);
        Buffer.clear(buffer);
        Event.dispatch(pipe_onData, out);
      };

      while (isReading^) {
        let ready =
          switch (Thread.wait_timed_read(pipe, 0.01)) {
          | exception _ =>
            isReading := false;
            false;
          | v => v
          };
        if (ready) {
          let n =
            switch (Unix.read(pipe, bytes, 0, 8192)) {
            | exception _ =>
              isReading := false;
              0;
            | v => v
            };

          if (n > 0) {
            let sub = Bytes.sub(bytes, 0, n);
            Buffer.add_bytes(buffer, sub);

            if (n < 8192) {
              flush();
            };
          } else if (! isRunning^) {
            if (Buffer.length(buffer) > 0) {
              flush();
            };
            isReading := false;
          };
        };
      };

      _safeClose(pipe);
    },
    (pipe, pipe_onData),
  );

let _spawn =
    (
      cmd: string,
      args: array(string),
      env: EnvironmentVariables.t,
      cwd: option(string),
    ) => {
  let (pstdin, stdin) = Unix.pipe();
  let (stdout, pstdout) = Unix.pipe();
  let (stderr, pstderr) = Unix.pipe();

  Unix.set_close_on_exec(pstdin);
  Unix.set_close_on_exec(stdin);
  Unix.set_close_on_exec(pstdout);
  Unix.set_close_on_exec(stdout);
  Unix.set_close_on_exec(pstderr);
  Unix.set_close_on_exec(stderr);

  let formattedEnv = _formatEnvironmentVariables(env);

  let pid =
    _withWorkingDirectory(cwd, () =>
      Unix.create_process_env(
        cmd,
        Array.append([|cmd|], args),
        formattedEnv,
        pstdin,
        pstdout,
        pstderr,
      )
    );

  Unix.close(pstdout);
  Unix.close(pstdin);
  Unix.close(pstderr);

  let stdout_onData = Event.create();
  let stderr_onData = Event.create();
  let onClose = Event.create();

  let isRunning = ref(true);

  let readThread = createReadingThread(stdout, stdout_onData, isRunning);
  let errThread = createReadingThread(stderr, stderr_onData, isRunning);

  let _dispose = exitCode => {
    isRunning := false;
    Event.dispatch(onClose, exitCode);
    _safeClose(stdin);

    /*_safeClose(stdout);
    _safeClose(stderr);*/
    Gc.full_major();
  };

  let waitThread =
    Thread.create(
      pid => {
        open Unix;
        let (_, status) = Thread.wait_pid(pid);
        let exitCode =
          switch (status) {
          | WEXITED(x) => x
          | WSIGNALED(x) => x
          | WSTOPPED(x) => x
          };
        _dispose(exitCode);
      },
      pid,
    );

  let retStdout: outputPipe = {onData: stdout_onData};
  let retStderr: outputPipe = {onData: stderr_onData};

  let stdinClose = () => {
    Unix.close(stdin);
  };

  let stdinWrite = bytes => {
    switch (Unix.write(stdin, bytes, 0, Bytes.length(bytes))) {
    /* A write may fail expectedly if the process was closed internally.
       In that case, the write channel will be invalid prior to us
       getting a close event - so we should just ignore it. */
    | exception (Unix.Unix_error(_)) => ()
    | _ => ()
    };
  };

  let retStdin: inputPipe = {write: stdinWrite, close: stdinClose};

  let kill = sig_ =>
    if (isRunning^) {
      let signalToUse =
        Sys.win32
          ? Sys.sigkill  /* Sigkill is the only signal supported on Win by the Unix module */
          : sig_;

      _safeClose(stdin);
      _safeClose(stdout);
      _safeClose(stderr);

      /* In some cases, we get a Unix_error(Unix.ESRCH, "kill") exception thrown */
      /* For now - just ignore. Is there a case where the consumer needs to know / handle this? */
      switch (Unix.kill(pid, signalToUse)) {
      | exception (Unix.Unix_error(_)) => ()
      | _ => ()
      };
    };

  let ret: innerProcess = {
    pid,
    stdin: retStdin,
    stdout: retStdout,
    stderr: retStderr,
    onClose,
    exitCode: ref(None),
    kill,
    _waitThread: waitThread,
    _readThread: readThread,
    _errThread: errThread,
  };

  let _ = Event.subscribe(onClose, code => ret.exitCode := Some(code));

  ret;
};

let spawn =
    (
      ~cwd=None,
      ~env=EnvironmentUtility.getEnvironmentVariables(),
      cmd: string,
      args: array(string),
    ) => {
  let {pid, kill, stdin, stdout, stderr, onClose, exitCode, _} =
    _spawn(cmd, args, env, cwd);

  let ret: process = {pid, kill, stdin, stdout, stderr, onClose, exitCode};
  ret;
};

let spawnSync =
    (
      ~cwd=None,
      ~env=EnvironmentUtility.getEnvironmentVariables(),
      ~opts=SpawnSyncOptions.default,
      cmd: string,
      args: array(string),
    ) => {
  let innerProc = _spawn(cmd, args, env, cwd);

  let outOutput = ref("");
  let errOutput = ref("");
  let outUnsubscribe =
    Event.subscribe(innerProc.stdout.onData, data =>
      outOutput := outOutput^ ++ Bytes.to_string(data)
    );

  let errUnsubscribe =
    Event.subscribe(innerProc.stderr.onData, data =>
      errOutput := errOutput^ ++ Bytes.to_string(data)
    );

  switch (opts.input) {
  | Some(x) => innerProc.stdin.write(Bytes.of_string(x))
  | None => ()
  };

  innerProc.stdin.close();

  Thread.join(innerProc._waitThread);
  Thread.join(innerProc._readThread);
  Thread.join(innerProc._errThread);
  outUnsubscribe();
  errUnsubscribe();

  let exitCode =
    switch (innerProc.exitCode^) {
    | Some(x) => x
    | None => (-1)
    };

  let ret: processSync = {
    pid: innerProc.pid,
    stdout: outOutput^,
    stderr: errOutput^,
    exitCode,
  };
  ret;
};
