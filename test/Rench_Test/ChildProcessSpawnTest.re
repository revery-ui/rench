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

describe("ChildProcess", ({test, describe}) => {
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
      let expectedPath = Sys.getenv("PATH");
      let script = {|
                console.log(process.env.PATH);
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
  });
});
