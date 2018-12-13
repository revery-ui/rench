let pathSeparator = Sys.win32 ? ";" : ":";

let dirname = (s) => {
    Fpath.v(s)
    |> Fpath.parent
    |> Fpath.to_string
};
