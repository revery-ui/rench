/* Code inspired by Esy:
 * https://github.com/esy/esy/blob/31cf81d39bcb6d83043f0444e85afbe9ec64c5ea/esy-lib/Environment.ml#L184
 */
let parseEnv = (item: string) => {
  let idx = String.index(item, '=');
  let name = String.sub(item, 0, idx);
  let name =
    switch (Sys.win32) {
    | true => String.uppercase_ascii(name)
    | _ => name
    };
  let value = String.sub(item, idx + 1, String.length(item) - idx - 1);
  (name, value);
};
