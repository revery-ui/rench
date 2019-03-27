/* 
 * Unix.getpid() on Windows doesn't return the expected result...
 * See: https://github.com/ocaml/ocaml/issues/4034
 */

external _win32_pid: unit => unit = "rench_win32_pid";

let pid = () => switch(Sys.win32) {
    | true => _win32_pid();
    | false => Unix.getpid();
};
