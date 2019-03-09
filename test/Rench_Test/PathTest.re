open TestFramework;

module Path = Rench.Path;

let testIfWindows = (test, s, f) =>
  if (Sys.win32) {
    test(s, f);
  };

describe("Path", ({describe, _}) => {
  describe("join", ({test, _}) => {

      if (Sys.win32) {
        test("forward slash path", ({expect}) => {
          let p1 = "C:\\test1\\test2";
          let p2 = ".\\test3\\test4.json";
          expect.string(Path.join(p1, p2)).toEqual("C:\\test1\\test2\\test3\\test4.json");
        });

        test("backslash path", ({expect}) => {
          let p1 = "C:/test1/test2";
          let p2 = "./test3/test4.json";
          expect.string(Path.join(p1, p2)).toEqual("C:\\test1\\test2\\test3\\test4.json");
        });

        test("relative path", ({expect}) => {
          let p1 = "test1/test2";
          let p2 = "./test3/test4.json";
          expect.string(Path.join(p1, p2)).toEqual("test1\\test2\\test3\\test4.json");
        });
      } else {
        test("backslash path", ({expect}) => {
          let p1 = "/test1/test2";
          let p2 = "./test3/test4.json";
          expect.string(Path.join(p1, p2)).toEqual("/test1/test2/test3/test4.json");
        });

        test("relative path", ({expect}) => {
          let p1 = "test1/test2";
          let p2 = "./test3/test4.json";
          expect.string(Path.join(p1, p2)).toEqual("test1/test2/test3/test4.json");
        });
      }

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
