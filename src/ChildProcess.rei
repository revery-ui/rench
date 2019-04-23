module SpawnSyncOptions = ChildProcessTypes.SpawnSyncOptions;

type inputFunction = Bytes.t => unit;
type closeFunction = int => unit;

type pid = int;
type exitCode = int;

let spawn:
  (
    ~cwd: option(string)=?,
    ~env: EnvironmentVariables.t=?,
    ~onStdout: inputFunction,
    ~onClose: closeFunction,
    string,
    array(string)
  ) =>
  (int, inputFunction, closeFunction);

let spawnSync:
  (
    ~cwd: option(string)=?,
    ~env: EnvironmentVariables.t=?,
    ~opts: SpawnSyncOptions.t=?,
    string,
    array(string)
  ) =>
  (pid, exitCode, string);
