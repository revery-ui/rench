let pathSeparator = Sys.win32 ? ";" : ":";

let dirname = s => {
  Fpath.v(s) |> Fpath.parent |> Fpath.rem_empty_seg |> Fpath.to_string;
};

let normalize = s => {
  Fpath.v(s) |> Fpath.to_string;
};

let filename = s => {
  Fpath.v(s) |> Fpath.filename;
};

let extname = s => {
  Fpath.v(s) |> Fpath.get_ext;
};

let join = (p1, p2) => {
  let p1 = Fpath.v(p1) |> Fpath.normalize;
  let p2 = Fpath.v(p2) |> Fpath.normalize;
  Fpath.append(p1, p2) |> Fpath.normalize |> Fpath.to_string;
};
