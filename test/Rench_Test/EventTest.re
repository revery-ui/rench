open TestFramework;

module Event = Rench.Event;

describe("Event", ({test, _}) => {
  test("dispatch", ({expect, _}) => {
    let evt: Event.t(int) = Event.create();

    let c = ref(0);
    let f = v => {
      c := v;
    };

    let _: Event.unsubscribe = Event.subscribe(evt, f);
    Event.dispatch(evt, 1);

    expect.int(c^).toBe(1);

    Event.dispatch(evt, 2);

    expect.int(c^).toBe(2);
  });

  test("unsubscribe", ({expect, _}) => {
    let evt: Event.t(int) = Event.create();

    let c = ref(0);
    let f = v => {
      c := v;
    };

    let unsubscribe = Event.subscribe(evt, f);
    Event.dispatch(evt, 1);
    unsubscribe();

    Event.dispatch(evt, 2);

    expect.int(c^).toBe(1);
  });
});
