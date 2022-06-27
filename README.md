# MzingaCpp #

[![CI Build](https://github.com/jonthysell/MzingaCpp/actions/workflows/ci.yml/badge.svg)](https://github.com/jonthysell/MzingaCpp/actions/workflows/ci.yml)

MzingaCpp is an [Universal Hive Protocol](https://github.com/jonthysell/Mzinga/wiki/UniversalHiveProtocol) Engine written in C++.

It implements the base game and the three official expansions.

## Installation ##

### Windows ###

The Windows release provides self-contained x86/x64 binaries which run on Windows.

1. Download the latest Windows zip file from https://github.com/jonthysell/MzingaCpp/releases/latest
2. Extract the zip file

**Note:** If you're unsure which version to download, try x64. Most modern PCs are 64-bit.

### MacOS ###

The MacOS release provides self-contained x64 binaries which run on OSX.

1. Download the latest MacOS tar.gz file from https://github.com/jonthysell/MzingaCpp/releases/latest
2. Extract the tar.gz file

### Linux ###

The Linux release provides self-contained x64 binaries which run on many Linux distributions.

1. Download the latest Linux tar.gz file from https://github.com/jonthysell/MzingaCpp/releases/latest
2. Extract the tar.gz file

## Build ##

This project requires CMake >= 3.16 and a standard C++ build environment.

### Windows (VS) ###

Open VS Command Prompt, then run one of the following:

* Build Debug: `.\scripts\build-debug.cmd`
* Build Release: `.\scripts\build-release.cmd`

### Linux (GCC) / MacOS (AppleClang) ###

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

Copyright (c) 2020-2022 Jon Thysell.

Hive Copyright (c) 2016 Gen42 Games. This repo is in no way associated with or endorsed by Gen42 Games.
