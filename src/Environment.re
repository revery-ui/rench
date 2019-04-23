/*
 * Environment
 */

let which = (executableName: string) => {
  /* TODO: An alternative implementation would be to go through the PATH */
  let whichCommand = Sys.win32 ? "where" : "which";

  let (_, exitCode, stdout, _) = ChildProcess.spawnSync(whichCommand, [|executableName|]);

  switch (exitCode) {
  | 0 =>
    let v =
      stdout
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
};

let getWorkingDirectory = () => {
  Sys.getcwd();
};

let getExecutingDirectory = () => {
  Filename.dirname(Sys.argv[0]);
};
