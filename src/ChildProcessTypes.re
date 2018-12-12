type closeFunction = unit => unit;
type writeFunction = Bytes.t => unit;

type inputPipe = {
    write: writeFunction,
    close: closeFunction
};

type outputPipe = {onData: Event.t(Bytes.t)};

type process = {
  pid: int,
  stdout: outputPipe,
  stdin: inputPipe,
  onClose: Event.t(int),
  exitCode: ref(option(int)),
};

type processSync = {
  pid: int,
  exitCode: int,
  stdout: string,
};

module SpawnSyncOptions = {
  type t = {input: option(string)};

  let default: t = {input: None};

  let create = (~input=?, ()) => {
    let ret: t = {input: input};
    ret;
  };
};
