type process = ChildProcessTypes.process;
type processSync = ChildProcessTypes.processSync;

module SpawnSyncOptions = ChildProcessTypes.SpawnSyncOptions;

let spawn: (string, array(string)) => process;
let spawnSync: (~opts: SpawnSyncOptions.t=?, string, array(string)) => processSync;
