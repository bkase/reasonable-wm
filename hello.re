/* vim: set ft=reason */

/* TODO: Why do I need to rewrite this? */
let module Option = {
  let map: ('a => 'b) => option 'a => option 'b = fun f =>
    fun
    | None => None
    | Some x => Some (f x);
};

let module Z = {
  type t 'a = | Zipper of (list 'a) 'a (list 'a);

  let one = fun x => Zipper [] x [];
  
  /* returns (list, idx) */
  let toList = fun
    | Zipper l p r => (l @ [p, ...r], List.length l);

  let extract = fun
    | Zipper l p r => p;

  let rec leftBy = fun cnt z => switch (z, cnt) {
    | (Zipper [] p r, _) => z
    | (_, 0) => z
    | (Zipper [x, ...xs] p r, c) =>
        leftBy (c-1) (Zipper xs x [p, ...r]) 
  };

  let rec rightBy = fun cnt z => switch (z, cnt) {
    | (Zipper l p [], _) => z
    | (_, 0) => z
    | (Zipper l p [x, ...xs], c) =>
        rightBy (c-1) (Zipper [p, ...l] x xs)
  };

  let left = leftBy 1;
  let right = rightBy 1;

  let leftmost = fun z => switch z {
    | Zipper l _ _ => leftBy (List.length l) z;
  };

  let rightmost = fun z => switch z {
    | Zipper _ _ r => rightBy (List.length r) z
  };

  let pushOnRight = fun x => fun
    | Zipper l p r => Zipper l x [p, ...r];

  let pushOnLeft = fun x => fun
    | Zipper l p r => Zipper [p, ...l] x r;

  let map = fun f (Zipper l p r) =>
    Zipper (List.map f l) (f p) (List.map f r);

  let length = fun
    | Zipper l p r => (List.length l) + 1 + (List.length r);


  /* from l pointing to idx */
  /* raises if list is empty */
  let fromList: int => list 'a => t 'a = fun idx l => switch l {
    | [] => raise (Invalid_argument "list is empty")
    | [x, ...xs] => {
      let folder: t 'a => 'a => t 'a = fun z elem =>
          z |> pushOnLeft elem;

      let rightmostFromList: t 'a =
        List.fold_left(folder) (one x) xs;

      rightmostFromList |> leftBy (List.length xs)
    }
  };
};
let module WinId = {
  type t = int;
  let _cnt: ref int = ref (-1);
  
  let next: unit => t = fun () => {
    _cnt := (!_cnt) + 1;
    !_cnt
  };
};

type style =
  | BigLeft;
/* a bounding rect in float between 0 and 1 */
let module Rect = {
  type t = {
    x: float,
    y: float,
    width: float,
    height: float
  };
  let all: t = {
    x: 0.0,
    y: 0.0,
    width: 1.0,
    height: 1.0
  };
  let vertHalve: t => t = fun
    | {x, y, width, height} => {
      x, y, height,
      width: width /. 2.0,
    };
  let horizHalve: t => t = fun
    | {x, y, width, height} => {
      x, y, width,
      height: height /. 2.0,
    };
};
/* Workspace */
type workspace 'a = option (Z.t 'a);
type windows = workspace WinId.t;
type layout = workspace (WinId.t, Rect.t);

/* TODO: Why does this not work?
 *
 * let moveLeft: workspace 'a => workspace 'a = Option.map Z.left;
  let moveRight: workspace 'a => workspace 'a = Option.map Z.right;
  */
let moveLeft: windows => windows = Option.map Z.left;
let moveRight: windows => windows = Option.map Z.right;
let spawn: windows => windows = fun w => {
  let id = WinId.next();
  switch w {
    | None => Some (Z.one id)
    | Some z => Some (z |> Z.pushOnLeft id)
  }
};
let emptyWindows: windows = None;

let module Render = {
  let _drawBigLeft = fun w => {
    let (l, idx) = Z.toList w;
    switch l {
       /* shouldn't happen; should we make NonEmpty list? */
      | [] => None
      | [big, ...rest] => {
        let bigRect = Rect.vertHalve Rect.all;
        let restLen = List.length rest;
        let restRects: list Rect.t =
          rest |> List.mapi(fun idx x =>
            ({ x: 0.5,
              y: (1.0 /. (float_of_int restLen)) *. (float_of_int idx),
              width: 0.5,
              height: 1.0 /. (float_of_int restLen) } : Rect.t)
          );
        let zipped = List.combine l [bigRect, ...restRects];

        Some (zipped |> Z.fromList idx)
      }
    }
  };

  let draw: style => windows => layout = fun s w => switch (s, w) {
    | (_, None) => None
    | (BigLeft, Some z) => _drawBigLeft z
  };

  let undraw: layout => windows =
      Option.map (Z.map (fun (id, _) => id));
};

/* Pretend a user is inputting these commands */
let ws =
  emptyWindows |>
  spawn |>
  spawn |>
  spawn |>
  moveLeft |>
  spawn |>
  spawn |>
  Render.draw BigLeft;

Js.log(ws);

