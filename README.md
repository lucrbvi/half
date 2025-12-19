# Half

Half is a minimal functional programming language written in C++20 with a valid C99 API. It's based on untyped [lambda-calculus](https://en.wikipedia.org/wiki/Lambda_calculus).

Everything is a function. To interact with the environment Half is using Church's booleans with the builtin function `:show`. It means you can only interact in binary with the computer.

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
=> 0%
```

In Half there is only three things:
1. Values (eg: x, foo, bar, 15, ...)
2. Function declarator with `\`
3. Body (after a dot `.`), it can contain another function

To declare a function you need to use those three things: `x = \x.x`.
