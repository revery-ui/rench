open TestFramework;

module Path = Rench.Path;

let testIfWindows = (test, s, f) =>
  if (Sys.win32) {
    test(s, f);
  };

describe("Path", ({describe, _}) => {
  describe("join", ({test, _}) =>
    if (Sys.win32) {
      test("forward slash path", ({expect, _}) => {
        let p1 = "C:\\test1\\test2";
        let p2 = ".\\test3\\test4.json";
        expect.string(Path.join(p1, p2)).toEqual(
          "C:\\test1\\test2\\test3\\test4.json",
        );
      });

      test("backslash path", ({expect, _}) => {
        let p1 = "C:/test1/test2";
        let p2 = "./test3/test4.json";
        expect.string(Path.join(p1, p2)).toEqual(
          "C:\\test1\\test2\\test3\\test4.json",
        );
      });

      test("relative path", ({expect, _}) => {
        let p1 = "test1/test2";
        let p2 = "./test3/test4.json";
        expect.string(Path.join(p1, p2)).toEqual(
          "test1\\test2\\test3\\test4.json",
        );
      });
    } else {
      test("backslash path", ({expect, _}) => {
        let p1 = "/test1/test2";
        let p2 = "./test3/test4.json";
        expect.string(Path.join(p1, p2)).toEqual(
          "/test1/test2/test3/test4.json",
        );
      });

      test("relative path", ({expect, _}) => {
        let p1 = "test1/test2";
        let p2 = "./test3/test4.json";
        expect.string(Path.join(p1, p2)).toEqual(
          "test1/test2/test3/test4.json",
        );
      });
    }
  );

  describe("isAbsolute", ({test, _}) =>
    if (Sys.win32) {
      test("win32 - root path", ({expect, _}) =>
        expect.bool(Path.isAbsolute("D:/test.txt")).toBe(true)
      );

      test("win32 - relative path", ({expect, _}) =>
        expect.bool(Path.isAbsolute("test/test.txt")).toBe(false)
      );
    } else {
      test("home path", ({expect, _}) =>
        expect.bool(Path.isAbsolute("~/test.txt")).toBe(true)
      );
      test("root path", ({expect, _}) =>
        expect.bool(Path.isAbsolute("/test.txt")).toBe(true)
      );

      test("relative path", ({expect, _}) =>
        expect.bool(Path.isAbsolute("test/test.txt")).toBe(false)
      );
    }
  );

  describe("basename", ({test, _}) => {
    if (Sys.win32) {
      test("basic test case - Win32 forward slash", ({expect, _}) => {
        let path = "C:/test/test1.html";
        expect.string(Path.filename(path)).toEqual("test1.html");
      });

      test("basic test case - Win32 backslash", ({expect, _}) => {
        let path = "C:\\test\\test1.html";
        expect.string(Path.filename(path)).toEqual("test1.html");
      });
    };

    test("basic test case - POSIX forward slash", ({expect, _}) => {
      let path = "/test/test1.html";
      expect.string(Path.filename(path)).toEqual("test1.html");
    });
  });
  describe("extname", ({test, _}) => {
    if (Sys.win32) {
      test("basic test case - Win32 forward slash", ({expect, _}) => {
        let path = "C:/test/test1.html";
        expect.string(Path.extname(path)).toEqual(".html");
      });

      test("basic test case - Win32 backslash", ({expect, _}) => {
        let path = "C:\\test\\test1.html";
        expect.string(Path.extname(path)).toEqual(".html");
      });
    };

    test("basic test case - POSIX forward slash", ({expect, _}) => {
      let path = "/test/test1.html";
      expect.string(Path.extname(path)).toEqual(".html");
    });

    test("multiple '.'", ({expect, _}) => {
      let path = "index.coffee.md";
      expect.string(Path.extname(path)).toEqual(".md");
    });

    test("dotfile", ({expect, _}) => {
      let path = ".index";
      expect.string(Path.extname(path)).toEqual("");
    });
  });
  describe("dirname", ({test, _}) => {
    test("forward slash path", ({expect, _}) => {
      let path = "C:/test/myfile.txt";
      expect.string(Path.dirname(path)).toEqual(Path.normalize("C:/test"));
    });

    testIfWindows(
      test,
      "windows-style backslash path",
      ({expect, _}) => {
        let path = "C:\\test\\myfile.txt";
        expect.string(Path.dirname(path)).toEqual(
          Path.normalize("C:\\test"),
        );
      },
    );

    test("node example", ({expect, _}) => {
      let path = Path.dirname("/foo/bar/baz/asdf/quux");
      expect.string(path).toEqual(Path.normalize("/foo/bar/baz/asdf"));
    });
  });
});
