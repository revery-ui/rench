open TestFramework;

module Path = Rench.Path;

let testIfWindows = (test, s, f) => {
    if (Sys.win32) {
        test(s, f);
    }
};

describe("Path", ({describe, _}) => {
    describe("dirname", ({test, _}) => {
      test("forward slash path", ({expect}) => {
          let path = "C:/test/myfile.txt";
          expect.string(Path.dirname(path)).toEqual(Path.normalize("C:/test"));
      });

      testIfWindows(test, "windows-style backslash path", ({expect}) => {
          let path = "C:\\test\\myfile.txt";
          expect.string(Path.dirname(path)).toEqual(Path.normalize("C:\\test"));
      });

      test("node example", ({expect}) => {
          let path = Path.dirname("/foo/bar/baz/asdf/quux");
          expect.string(path).toEqual(Path.normalize("/foo/bar/baz/asdf"));
      });
    });
});
