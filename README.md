# Half

Half is a minimal functional programming language written in a single C Header file. It's based on untyped [lambda-calculus](https://en.wikipedia.org/wiki/Lambda_calculus).

Everything is a function. To make Half useful we need to have some impure functions. These functions are called "builtins". There are several builtins:
- `:show`: send a single bit to the terminal based on the Church booleans it receives (0=false, 1=true).
- `:read`: return to the program a Church boolean based on the program call arguments or stdin channel (if there is no arguments).
- `:ast`: (DEBUG ONLY) show the AST of a function.

It means a Half program is a pure function (unless if you introduce non-determenistic builtins yourself!).

In the future I plan to introduce a standard library written in Half itself to avoid doing binary stuff manually for everything.

### Examples

Here is a program in Half that output "0" in the terminal:

```hl
# zero.hl

1 = \x.\y.x
0 = \x.\y.y

:show 0
:show 0
:show 1
:show 1
:show 0
:show 0
:show 0
:show 0
```

```bash
./half zero.hl
0%
```

## Get started with Half

### Use Half as a library

If you want to embed Half in your program you can just copy and paste the header (`./libhalf.h`) to use in your program! The vast majority of programming languages can import C code through FFI or other methods. You may need yo write a wrapper yourself to expose LibHalf to your language.

### Building Half

To build Half you need to have a [C](https://www.c-language.org/) compiler supporting C99 installed.

You can just build the `main.c` with your favourite compiler, and that's all!
