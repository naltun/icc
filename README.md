# icc

Interactive C Compiler

[![OpenBSD](https://img.shields.io/badge/-OpenBSD-%23FCC771?style=flat&logo=openbsd&logoColor=black)](https://img.shields.io/badge/-OpenBSD-%23FCC771?style=flat&logo=openbsd&logoColor=black)
[![C](https://img.shields.io/badge/c-%2300599C.svg?style=flat&logo=c&logoColor=white)](https://img.shields.io/badge/c-%2300599C.svg?style=flat&logo=c&logoColor=white)
[![Neovim](https://img.shields.io/badge/NeoVim-%2357A143.svg?&style=flat&logo=neovim&logoColor=white)](https://img.shields.io/badge/NeoVim-%2357A143.svg?&style=flat&logo=neovim&logoColor=white)

<img src="./screenshot.png" alt="icc (the Interactive C Compiler) running in a transparent terminal, with an image of Mt. Ararat as a desktop background" />

`icc` is a C REPL inspired by [Interactive GCC](https://github.com/alexandru-dinu/igcc/)
by Alexandru Dinu, which is a fork of [Interactive GCC](https://sourceforge.net/projects/igcc/)
by Andy Balaam.

The main difference is `icc` is coded in C (not Python), will only support the
C language, and is tightly integrated with Unix environments.

Please note, this is a work in progress. Contributions are welcomed.

## Install

To install `icc`, run `make && make install`.

## License

Proudly free software, this code is licensed under the terms of the Mozilla Public License, v.2.0.
