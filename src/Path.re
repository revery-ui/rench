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
