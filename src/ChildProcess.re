include ChildProcessTypes;

type innerProcess = {
  pid: int,
  stdout: outputPipe,
  stdin: inputPipe,
  onClose: Event.t(int),
  exitCode: ref(option(int)),
  _readThread: Thread.t,
  _waitThread: Thread.t,
};

let _formatEnvironmentVariables = (env: EnvironmentVariables.t) => {
    let f = (key, value, prev) => {
        Array.append(prev, [|key ++ "=" ++ value|]);
    };

    EnvironmentVariables.fold(~f, env, [||]);
};

let _spawn = (cmd: string, args: array(string), env: EnvironmentVariables.t) => {
  let (pstdin, stdin) = Unix.pipe();
  let (stdout, pstdout) = Unix.pipe();

  Unix.set_close_on_exec(pstdin);
  Unix.set_close_on_exec(stdin);
  Unix.set_close_on_exec(pstdout);
  Unix.set_close_on_exec(stdout);

  let formattedEnv = _formatEnvironmentVariables(env);

  let pid =
    Unix.create_process_env(
      cmd,
      Array.append([|cmd|], args),
      formattedEnv,
      pstdin,
      pstdout,
      Unix.stderr,
    );

  Unix.close(pstdout);
  Unix.close(pstdin);

  let stdout_onData = Event.create();
  let onClose = Event.create();

  let isRunning = ref(true);

  let readThread =
    Thread.create(
      ((stdout, stdout_onData)) => {
        let buffer = Buffer.create(8192);
        let bytes = Bytes.create(8192);

        while (isRunning^) {
          Thread.wait_read(stdout);
          let n = Unix.read(stdout, bytes, 0, 8192);

          if (n > 0) {
            let sub = Bytes.sub(bytes, 0, n);
            Buffer.add_bytes(buffer, sub);

            if (n < 8192) {
              let out = Buffer.to_bytes(buffer);
              Buffer.clear(buffer);
              Event.dispatch(stdout_onData, out);
            };
          };
        };
      },
      (stdout, stdout_onData),
    );

  let _dispose = exitCode => {
    isRunning := false;
    Event.dispatch(onClose, exitCode);
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

  let stdinClose = () => {
    Unix.close(stdin);
  };

  let stdinWrite = bytes => {
    let _ = Unix.write(stdin, bytes, 0, Bytes.length(bytes));
    ();
  };

  let retStdin: inputPipe = {write: stdinWrite, close: stdinClose};

  let ret: innerProcess = {
    pid,
    stdin: retStdin,
    stdout: retStdout,
    onClose,
    exitCode: ref(None),
    _waitThread: waitThread,
    _readThread: readThread,
  };

  let _ = Event.subscribe(onClose, code => ret.exitCode := Some(code));

  ret;
};

let spawn = (~env=EnvironmentUtility.getEnvironmentVariables(), cmd: string, args: array(string)) => {
  let {pid, stdin, stdout, onClose, exitCode, _} = _spawn(cmd, args, env);

  let ret: process = {pid, stdin, stdout, onClose, exitCode};
  ret;
};

let spawnSync =
    (~env=EnvironmentUtility.getEnvironmentVariables(), ~opts=SpawnSyncOptions.default, cmd: string, args: array(string)) => {
  let innerProc = _spawn(cmd, args, env);

  let output = ref("");
  let unsubscribe =
    Event.subscribe(innerProc.stdout.onData, data =>
      output := output^ ++ Bytes.to_string(data)
    );

  switch (opts.input) {
  | Some(x) =>
    innerProc.stdin.write(Bytes.of_string(x));
    innerProc.stdin.close();
  | None => ()
  };

  Thread.join(innerProc._waitThread);
  Thread.join(innerProc._readThread);
  unsubscribe();

  let exitCode =
    switch (innerProc.exitCode^) {
    | Some(x) => x
    | None => (-1)
    };

  let ret: processSync = {pid: innerProc.pid, stdout: output^, exitCode};
  ret;
};
