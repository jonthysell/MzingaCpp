# MzingaCpp #

[![CI Build](https://github.com/jonthysell/MzingaCpp/actions/workflows/ci.yml/badge.svg)](https://github.com/jonthysell/MzingaCpp/actions/workflows/ci.yml)

MzingaCpp is an [Universal Hive Protocol](https://github.com/jonthysell/Mzinga/wiki/UniversalHiveProtocol) Engine written in C++.

It implements the base game and the three official expansions.

## Build ##

This project requires CMake >= 3.16 and a standard C++ build environment.

### Windows (VS 2019) ###

* Build Debug: `.\scripts\build-debug.cmd`
* Build Release: `.\scripts\build-release.cmd`

### Linux (gcc) ###

* Build Debug: `./scripts/build-debug.sh`
* Build Release: `./scripts/build-release.sh`

### General ###

```
mkdir build
cd build
cmake ..
cmake --build .
```

## Errata ##

MzingaCpp is open-source under the MIT license.

Copyright (c) 2020-2021 Jon Thysell.

Hive Copyright (c) 2016 Gen42 Games. This repo is in no way associated with or endorsed by Gen42 Games.
