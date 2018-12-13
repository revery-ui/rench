/*
 * EnvironmentVariables
 */

type environmentVariable = (string, string);

module StringMap = Map.Make(String);

type t = StringMap.t(string);

let create = (v: list(environmentVariable)) => {
  let f = (prev: t, e: environmentVariable) => {
    let (name, value) = e;
    StringMap.add(name, value, prev);
  };

  List.fold_left(f, StringMap.empty, v);
};

let getValue = (env: t, name: string) => {
  StringMap.find_opt(name, env);
};

let setValue = (env: t, name: string, v: string) => {
  StringMap.add(name, v, env);
};

let fold = (~f, env: t, curr: 'a) => {
  StringMap.fold(f, env, curr);
};
