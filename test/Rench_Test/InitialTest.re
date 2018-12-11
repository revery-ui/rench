
open TestFramework;

describe("test", ({test, _}) => {
  test("Hello, world", ({expect}) => {
      expect.string("a").toEqual("a");
  });
});
