type process = ChildProcessTypes.process;
type processSync = ChildProcessTypes.processSync;

module SpawnSyncOptions = ChildProcessTypes.SpawnSyncOptions;

let spawn: (~cwd:option(string)=?, ~env: EnvironmentVariables.t=?, string, array(string)) => process;
let spawnSync: (~cwd:option(string)=?, ~env: EnvironmentVariables.t=?, ~opts: SpawnSyncOptions.t=?, string, array(string)) => processSync;
