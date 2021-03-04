/* Fs.re */

let readFileSync = (path) => {
    let chan = open_in_bin(path);
    let data = ref("");
    try ({
        while (true) {
            data := data ++ "\n" ++ input_line(chan);
        } 
        data;
    }) {
    | End_of_file => {
        close_in(chan);
        data;
    }
    }:
};
