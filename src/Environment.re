/*
 * Environment
 */

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
