open TestFramework;

module ChildProcess = Rench.ChildProcess;
module Environment = Rench.Environment;
module EnvironmentVariables = Rench.EnvironmentVariables;
module Event = Rench.Event;

let waitFor = f => {
  while (!f()) {
    Unix.sleepf(0.05);
  };
};

let waitForProcessExit = (proc: ChildProcess.process) => {
  waitFor(() =>
    switch (proc.exitCode^) {
    | None => false
    | Some(_) => true
    }
  );
};

describe("ChildProcess", ({test, describe, _}) => {
  test("spawn", ({expect}) => {
    let script = {|
        console.log('v1000');
    |};
    let proc = ChildProcess.spawn("node", [|"-e", script|]);

    let data = ref("");
    let _ =
      Event.subscribe(proc.stdout.onData, str =>
        data := data^ ++ Bytes.to_string(str)
      );

    waitForProcessExit(proc);

    /* Check that we got _some_ version */
    expect.string(data^).toEqual("v1000\n");

    switch (proc.exitCode^) {
    | Some(x) => expect.int(x).toBe(0)
    | _ => ()
    };
  });

  test("spawn with output to stderr", ({expect}) => {
    /* Some invalid command that outputs to stderr */
    let proc = ChildProcess.spawn("ls", [|"-?"|]);
    let expectedOutput = [
      "ls: invalid option -- '?'",
      "Try 'ls --help' for more information."
    ]

    let data = ref("");
    let _ =
      Event.subscribe(proc.stderr.onData, str =>
        data := data^ ++ Bytes.to_string(str)
      );

    waitForProcessExit(proc);

    let formattedData = Str.split(Str.regexp("\n"), data^)

    /* Check we got the expected error message */
    expect.list(formattedData).toEqual(expectedOutput);

    switch (proc.exitCode^) {
    | Some(x) => expect.notEqual(0, x) /* Should have errored */
    | _ => ()
    };
  });

  describe("spawnSync", ({test, _}) => {
    test("process creation", ({expect}) => {
      let script = {|
            console.log('v1000');
        |};

      let proc = ChildProcess.spawnSync("node", [|"-e", script|]);

      expect.string(proc.stdout).toEqual("v1000\n");
      expect.int(proc.exitCode).toBe(0);
    });

    test("i/o via readline", ({expect}) => {
      /* Create a simple echo script */
      let script = {|
        let readline = require('readline');
        let rl = readline.createInterface({
            input: process.stdin,
            output: process.stdout,
            terminal: false,
        });
        rl.on('line', (l) => {
            console.log(l);
            process.exit(0);
        })
      |};

      let opts =
        ChildProcess.SpawnSyncOptions.create(~input="Testing 123\n", ());
      let proc = ChildProcess.spawnSync(~opts, "node", [|"-e", script|]);

      expect.string(proc.stdout).toEqual("Testing 123\n");
      expect.int(proc.exitCode).toBe(0);
    });

    test("i/o via pipes", ({expect}) => {
      /* Create a simple echo script */
      let script = {|
        process.stdin.on("data", (d) => {
            console.log(d.toString("utf8"));
        });
      |};

      let opts =
        ChildProcess.SpawnSyncOptions.create(~input="Testing 456\n", ());
      let proc = ChildProcess.spawnSync(~opts, "node", [|"-e", script|]);

      expect.string(String.trim(proc.stdout)).toEqual("Testing 456");
      expect.int(proc.exitCode).toBe(0);
    });

    test("default environment variable set if nothing passed", ({expect}) => {
      /* OCAML_TOPLEVEL_PATH is set by default */
      let expectedPath = Sys.getenv("OCAML_TOPLEVEL_PATH");
      let script = {|
                console.log(process.env["OCAML_TOPLEVEL_PATH"]);
          |};

      let out =
        ChildProcess.spawnSync("node", [|"-e", script|])
        |> (p => p.stdout |> String.trim);

      expect.string(out).toEqual(expectedPath);
    });

    test("picks up set environment variable", ({expect}) => {
      let script = {|
                console.log(process.env.RENCH_TEST_VARIABLE);
          |};

      let currEnv = Environment.getEnvironmentVariables();
      let env =
        EnvironmentVariables.setValue(currEnv, "RENCH_TEST_VARIABLE", "0451");

      let out =
        ChildProcess.spawnSync(~env, "node", [|"-e", script|])
        |> (p => p.stdout |> String.trim);

      expect.string(out).toEqual("0451");
    });

    test("handles large amounts of output", ({expect}) => {
      let script = {|
            const ITERATIONS = 1000001;
            for (let i = 0; i < ITERATIONS; i++) {
                console.log("WRITING LINE: --------|" + i.toString() + "|--------");
            }
          |};

      let out =
        ChildProcess.spawnSync("node", [|"-e", script|])
        |> (p => p.stdout |> String.trim);

      let str = String.split_on_char('\n', out);

      let last = str |> List.rev |> List.hd;

      expect.string(last).toEqual("WRITING LINE: --------|1000000|--------");
      expect.int(List.length(str)).toBe(1000001);
    });

    exception NoNodeAvailable(string);

    test("respects cwd", ({expect}) => {
      let nodeDirectory =
        Rench.Environment.which("node")
        |> (
          v =>
            switch (v) {
            | Some(d) => d
            | None => raise(NoNodeAvailable("unable to find node"))
            }
        )
        |> Rench.Path.dirname;

      let script = {|
            console.log(process.cwd())
        |};

      let out =
        ChildProcess.spawnSync(
          ~cwd=Some(nodeDirectory),
          "node",
          [|"-e", script|],
        )
        |> (p => p.stdout)
        |> String.trim;

      expect.string(out).toEqual(nodeDirectory);
    });
  });
});
