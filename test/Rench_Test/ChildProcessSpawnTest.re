
open TestFramework;

module ChildProcess = Rench.ChildProcess;
module Event = Rench.Event;

let waitFor = (f) => {
    while (!f()) {
        Unix.sleepf(0.05);
    }
};

let waitForProcessExit = (proc: ChildProcess.t) => {
    waitFor(() => {
        switch (proc.exitCode) {
        | None => false
        | Some(_) => true
        }
    });
};

describe("ChildProcess", ({test, _}) => {
  test("spawn", ({expect}) => {

      let proc = ChildProcess.spawn("node", [|"-e", "console.log('v1000')"|]);

      let data = ref("");
      let _ = Event.subscribe(proc.stdout.onData, (str) => {
          data := data^ ++ Bytes.to_string(str);
      });

      waitForProcessExit(proc);

      /* Check that we got _some_ version */
      expect.string(String.sub(data^, 0, 1)).toEqual("v");

      switch (proc.exitCode) {
      | Some(x) => expect.int(x).toBe(0)
      | _ => ()
      };
  });

});
