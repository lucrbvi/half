# The Half Language Reference

## Introduction

Half is an untyped lambda-calculus based embeddable scripting language. It is designed to ensure that a script is a pure function.

It's also lazy by design. It means it will not evaluate your code unless you call a builtin with it.

### Syntax

Half is inspired by Python and have a minimal syntax.

The core element of Half is the function. To declare a function you must use the backslash ('\'), declare the name of the parameter then use a dot and then write the body of the function.

```hl
\name.body
```

Variables are just stores for functions.

```hl
true = \t.\f.t
```

And finally, there is the builtins functions. Those functions have side-effects, they execute a C function in the Half Interpreter when calling them.

To call a builtin function you must use the ':' character followed by its name. You can use a builtin as an argument for a function (they are still functions!).

And thats it! You've learned the whole Half programming language;
