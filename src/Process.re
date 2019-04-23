/*
 * Unix.getpid() on Windows doesn't return the expected result...
 * See: https://github.com/ocaml/ocaml/issues/4034
 */

external _rench_pid: int => int = "rench_pid";

let pid = () => _rench_pid(Unix.getpid());
