open TestFramework;

module Path = Rench.Path;

let testIfWindows = (test, s, f) =>
  if (Sys.win32) {
    test(s, f);
  };

describe("Path", ({describe, _}) => {
  describe("basename", ({test, _}) => {

      if (Sys.win32) {
          test("basic test case - Win32 forward slash", ({expect}) => {
            let path = "C:/test/test1.html";
            expect.string(Path.filename(path)).toEqual("test1.html");
          });

          test("basic test case - Win32 backslash", ({expect}) => {
            let path = "C:\\test\\test1.html";
            expect.string(Path.filename(path)).toEqual("test1.html");
          });
      }

      test("basic test case - POSIX forward slash", ({expect}) => {
        let path = "/test/test1.html";
        expect.string(Path.filename(path)).toEqual("test1.html");
      });
  });
  describe("extname", ({test, _}) => {
      if (Sys.win32) {
          test("basic test case - Win32 forward slash", ({expect}) => {
            let path = "C:/test/test1.html";
            expect.string(Path.extname(path)).toEqual(".html");
          });

          test("basic test case - Win32 backslash", ({expect}) => {
            let path = "C:\\test\\test1.html";
            expect.string(Path.extname(path)).toEqual(".html");
          });
      }

      test("basic test case - POSIX forward slash", ({expect}) => {
        let path = "/test/test1.html";
        expect.string(Path.extname(path)).toEqual(".html");
      });

      test("multiple '.'", ({expect}) => {
        let path = "index.coffee.md"; 
        expect.string(Path.extname(path)).toEqual(".md");
      });

      test("dotfile", ({expect}) => {
        let path = ".index"; 
        expect.string(Path.extname(path)).toEqual("");
      });
  });
  describe("dirname", ({test, _}) => {
    test("forward slash path", ({expect}) => {
      let path = "C:/test/myfile.txt";
      expect.string(Path.dirname(path)).toEqual(Path.normalize("C:/test"));
    });

    testIfWindows(
      test,
      "windows-style backslash path",
      ({expect}) => {
        let path = "C:\\test\\myfile.txt";
        expect.string(Path.dirname(path)).toEqual(
          Path.normalize("C:\\test"),
        );
      },
    );

    test("node example", ({expect}) => {
      let path = Path.dirname("/foo/bar/baz/asdf/quux");
      expect.string(path).toEqual(Path.normalize("/foo/bar/baz/asdf"));
    });
  })
});
