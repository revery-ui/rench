type unsubscribe = unit => unit;

type t(_);

let create: unit => t(_);
let subscribe: (t('a), 'a => unit) => unsubscribe;
let dispatch: (t('a), 'a) => unit;
