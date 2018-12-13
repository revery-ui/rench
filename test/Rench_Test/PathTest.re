open TestFramework;

module Path = Rench.Path;

describe("Path", ({describe, _}) => {
    describe("dirname", ({test, _}) => {
      test("forward slash path", ({expect}) => {
          let path = "C:/test/myfile.txt";
          expect.string(Path.dirname(path)).toEqual("C:/test");
      });

      test("windows-style backslash path", ({expect}) => {
          let path = "C:\\test\\myfile.txt";
          expect.string(Path.dirname(path)).toEqual("C:\\test");
      });
    });
});
