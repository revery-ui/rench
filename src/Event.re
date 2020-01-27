/* Event.re */

type unsubscribe = unit => unit;

type t('a) = ref(list('a => unit));

let create = () => ref([]);

let subscribe = (event, callback) => {
  event := List.append(event^, [callback]);

  let unsubscribe = () => event := List.filter(cb => cb !== callback, event^);
  unsubscribe;
};

let dispatch = (event, message) => List.iter(cb => cb(message), event^);
