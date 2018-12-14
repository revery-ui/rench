/*
 * Environment
 */

let which = (executableName: string) => {
  /* TODO: An alternative implementation would be to go through the PATH */
  let whichCommand = Sys.win32 ? "where" : "which";

  let proc = ChildProcess.spawnSync(whichCommand, [|executableName|]);

  switch (proc.exitCode) {
  | 0 =>
    let v =
      proc.stdout
      |> String.trim
      |> String.split_on_char('\n')
      |> List.hd
      |> String.trim;
    Some(v);
  | _ => None
  };
};

let newLine = Sys.win32 ? "\r\n" : "\n";

let getEnvironmentVariables = EnvironmentUtility.getEnvironmentVariables;

let getEnvironmentVariable = (name: string) => {
    let env = getEnvironmentVariables();
    EnvironmentVariables.getValue(env, name);
}
