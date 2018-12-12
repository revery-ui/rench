/*
 * Environment
 */

let whichSync = (executableName: string) => {
  /* TODO: An alternative implementation would be to go through the PATH */
  let whichCommand = Sys.win32 ? "where" : "which";

  let proc = ChildProcess.spawnSync(whichCommand, [|executableName|]);
  String.trim(proc.stdout);
};

let newLine = Sys.win32 ? "\r\n" : "\n";
