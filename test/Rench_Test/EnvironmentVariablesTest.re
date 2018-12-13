open TestFramework;

module EnvironmentVariables = Rench.EnvironmentVariables;

describe("EnvironmentVariables", ({describe, _}) =>
  describe("get / set", ({test, _}) =>
    test("setting a value works", ({expect}) => {

        let defaultEnv = [("env1", "env1value")];

        let env = EnvironmentVariables.create(defaultEnv);

        let res = EnvironmentVariables.getValue(env, "env1");
        switch (res) {
        | Some(v) => expect.string(v).toEqual("env1value");
        | _ => expect.bool(false).toBeTrue();
        };

        let env = EnvironmentVariables.setValue(env, "env1", "newValue");

        let res = EnvironmentVariables.getValue(env, "env1");
        switch (res) {
        | Some(v) => expect.string(v).toEqual("newValue");
        | _ => expect.bool(false).toBeTrue();
        };
    })
  )
);
