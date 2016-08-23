# Reasonable Window Manager

> A functional tiling "window manager" for the web written in Reason

* [Idea](#Idea)
* [Work TODO](#TODO)
* [Build Instructions](#Build-Instructions)
* [Dev Instructions](#Dev-Instructions)

### Idea

Building a functional [tiling windowing manager](https://en.wikipedia.org/wiki/Tiling_window_manager) for the web seems like a simple (but not too simple) first project to play with Reason.

[See this blog post by donsbot for more information.](https://donsbot.wordpress.com/2007/05/17/roll-your-own-window-manager-tracking-focus-with-a-zipper/)


I'm using a design based on a functional Zipper:

```rust
type zipper 'a = Zipper of (list 'a) 'a (list 'a)
```

You are pointing at the middle element, and the stuff to the left of you is on the left list, the stuff to the right of you is on the right list.

A Zipper is a finite pointed 1D topological space where you can move the basepoint or focus left or right.

```rust
type workspace 'a = option (Z.t 'a);
```

In this case our workspace is either empty or has some things in it and we're focused on one of them. 

```rust
type windows = workspace WinId.t;
```

`windows` is a workspace of unique identifiers to window. We can move our focus to other windows and spawn new windows (appends to one side of the zipper). 

```rust
type layout = workspace (WinId.t, Rect.t);
```

The `layout` is a rendered workspace. `Rect.t` is just `{x, y, width, height}` or information on how to position the windows in a workspace.

```rust
let module Render = {
  /* … */
  let draw: style => windows => layout
};
```

A `layout` can be created by combining a style (how you want to draw the windows) with the windows you want to draw.

A Zipper can move the focus left and right and insert near the focus in `O(1)` time, so moving around and creating new windows is cheap. Laying out the windows requires having a global understanding of the workspace, so that takes `O(n)`.

This is a work in progress.

### TODO:

* Build (at least one) frontend renderer (react?)
* Use bucklescript magic to create JSON records

### Build Instructions

Make sure you [follow the instructions here to install Reason](https://github.com/facebook/reason) (tested on v0.0.6)

Then install [BuckleScript](https://github.com/bloomberg/bucklescript) in a local `node_modules/` (`mkdir -p node_modules` in this directory if necessary).

To build the `hello.js`:

```sh
# this calls the bucklescript compiler with the necessary options
./build.sh
```

To package the `out.js` for `index.html`:
(install browserify if necessary `npm install -g browserify`)

```sh
browserify hello.js > out.js
```

Then `open index.html` in any browser and (for now) open up the console to see the output.

### Dev Instructions

To compile with good error messages: install [BetterErrors](https://github.com/npm-ml/BetterErrors)) and run:

```sh
ocamlc -pp refmt -o _build/out -impl hello.re 2>&1 | huh
```

If it compiles (up to the `Js` usage), I then do `./build.sh` and look at the `hello.js` artifact.

