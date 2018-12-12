open TestFramework;

module ChildProcess = Rench.ChildProcess;
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

    test("input parameter", ({expect}) => {
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
  });
});
