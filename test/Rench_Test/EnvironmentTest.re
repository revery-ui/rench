open TestFramework;

module Environment = Rench.Environment;
module EnvironmentVariables = Rench.EnvironmentVariables;
module Path = Rench.Path;

describe("Environment", ({describe, _}) => {
  describe("which", ({test, _}) =>
    test("can find node executable", ({expect}) => {
      let nodePath = Environment.which("node");
      switch (nodePath) {
      | Some(v) =>
        let exists = Sys.file_exists(v);
        expect.bool(exists).toBe(true);
      | None => expect.bool(false).toBe(true)
      };
    })
  );
  describe("getEnvironmentVariables", ({test, _}) =>
    test("PATH is available", ({expect}) => {
      let env = Environment.getEnvironmentVariables();
      let expectedPath = Sys.getenv("PATH");

      let actualValue = EnvironmentVariables.getValue(env, "PATH");
      switch (actualValue) {
      | Some(v) => expect.string(v).toEqual(expectedPath)
      | _ => expect.bool(true).toBeFalse()
      };
    })
  );
  describe("getExecutingDirectory", ({test, _}) =>
    test("returns correct path", ({expect}) => {
      let testRunnerPath = Environment.which("TestRunner.exe");
      switch (testRunnerPath) {
      | Some(v) =>
        let expected = Path.dirname(v);
        let xd = Environment.getExecutingDirectory();
        expect.string(expected).toEqual(xd);
      | None => expect.bool(true).toBeFalse()
      };
    })
  );
});
