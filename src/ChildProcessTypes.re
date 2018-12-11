type writeFunction = Bytes.t => unit;

type inputPipe = {write: writeFunction};

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

