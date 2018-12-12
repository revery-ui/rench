open TestFramework;

module Environment = Rench.Environment;

describe("Environment", ({describe, _}) =>
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
  )
);
