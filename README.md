# MyShell

A lightweight Unix-like shell written in C from scratch.

## Features

* Built-in commands:

  * `cd`
  * `pwd`
  * `echo`
  * `touch`
  * `mkdir`
  * `date`
  * `env`
  * `setenv`
  * `unsetenv`
  * `help`
  * `exit`

* Command parsing and tokenization

* Execution of external programs using `fork()` and `execve()`

* PATH environment variable resolution

* Custom environment variable management

* Output redirection (`>` and `>>`)

* Support for quoted filenames

* Cross-platform directory creation support (Linux/Windows)


## Learning Objectives

This project was built to gain a deeper understanding of:

* Shell internals
* Process creation and execution
* Environment management
* Command parsing
* Memory management in C
* Linux system programming

## Build

```bash
make
./my_shell
```

## Author

Iradri

Inspired from [**Dev w/Sel**](https://www.youtube.com/@Dev_with_Sel)
