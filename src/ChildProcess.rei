type process = ChildProcessTypes.process;
type processSync = ChildProcessTypes.processSync;

let spawn: (string, array(string)) => process;
let spawnSync: (string, array(string)) => processSync;
