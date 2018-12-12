open TestFramework;

module Environment = Rench.Environment;

describe("Environment", ({describe, _}) => {
  describe("which", ({test, _}) => {
      test("can find node executable", ({expect}) => {
          let nodePath = Environment.which("node");
          let exists = Sys.file_exists(nodePath);
          expect.bool(exists).toBe(true);
      });
  });
});
