# Half

Half is a minimal functional programming language written in C++20 with a valid C99 API. It's based on untyped [lambda-calculus](https://en.wikipedia.org/wiki/Lambda_calculus).

Everything is a function. To make Half useful we need to have some impure functions. These functions are called "builtins". There are several builtins:
- `:show`: send a single bit to the terminal based on the Church booleans it receives (0=false, 1=true).
- `:read`: return to the program a Church boolean based on the program call arguments or stdin channel (if there is no arguments).
- `:ast`: (DEBUG ONLY) show the AST of a function.

We will introduce a standard library written in Half to provide some helpers.

It means a Half program is a pure function.

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

### Building Half

To build Half you need to have a [C++20](https://isocpp.org/) compiler and [Make](https://www.gnu.org/software/make/) installed.

If you do not use [Clang and Clang++](https://clang.llvm.org), please modify the Makefile to use your preferred compiler.

Then you can juste run `make` to build Half. It will produce three binary files in the `./dist` directory:
- `./dist/main` (from `./main.cpp`): interpreter and interactive shell
- `./dist/test` (from `./test.c`): a basic suite of tests
- `./dist/libhalf.o` (from `./libhalf.h` and `./libhalf.cpp`): the Half library

Then you can use Half how you want. Have fun!

# TODO

1. The Half interpreter is strict, we should become lazy (it makes writing Y/Z combinators easier)
