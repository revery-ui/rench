open TestFramework;

module Event = Rench.Event;

describe("Fs", ({test, _}) => {
  test("readFileSync", ({expect}) => {
      expect.int(0).toBe(1);
  });
});
