type writeFunction = (Bytes.t) => unit;

type inputPipe = {
    write: writeFunction
};

type outputPipe = {
    onData: Event.t(Bytes.t)
};

type process = {
    pid: int,
    stdout: outputPipe,
    stdin: inputPipe,
};

let spawn = (cmd: string, args: array(string)) => {

    let (pstdin, stdin) = Unix.pipe();
    let (stdout, pstdout) = Unix.pipe();

    Unix.set_close_on_exec(pstdin);
    Unix.set_close_on_exec(stdin);
    Unix.set_close_on_exec(pstdout);
    Unix.set_close_on_exec(stdout);

    let pid = Unix.create_process(cmd, Array.append([|cmd|], args), pstdin, pstdout, Unix.stderr);

    print_endline ("[DEBUG] Created process: " ++ string_of_int(pid));

    Unix.close(pstdout);
    Unix.close(pstdin);

    let stdout_onData = Event.create();

    let thread = Thread.create(((stdout, stdout_onData)) => {
        let buffer = Buffer.create(8192);
        let bytes = Bytes.create(8192);

        while (true) {
            print_endline("Waiting...");
            Thread.wait_read(stdout);
            let n = Unix.read(stdout, bytes, 0, 8192);
            print_endline("Read " ++ string_of_int(n) ++ "bytes");

            if (n > 0) {
                let sub = Bytes.sub(bytes, 0, n);
                Buffer.add_bytes(buffer, sub)

                print_endline("[DEBUG] Received " ++ string_of_int(n) ++ "bytes from process " ++ string_of_int(pid))

                if (n < 8192) {
                    let out = Buffer.to_bytes(buffer);
                    Buffer.clear(buffer);
                    Event.dispatch(stdout_onData, out);
                }
            } 
        }
    }, (stdout, stdout_onData));

    /* TODO: Hook this up to process close */
    let _dispose = () => {
        Thread.kill(thread);
    };

    let retStdout: outputPipe = {
        onData: stdout_onData,
    };

    let stdinWrite = (bytes) => {
        let n = Unix.write(stdin, bytes, 0, Bytes.length(bytes));
        print_endline ("Wrote " ++ string_of_int(n) ++ " bytes to process " ++ string_of_int(pid));
    };

    let retStdin: inputPipe = {
        write: stdinWrite,
    };

    let ret: process = {
        pid,
        stdin: retStdin,
        stdout: retStdout,
    };
    ret;
};
