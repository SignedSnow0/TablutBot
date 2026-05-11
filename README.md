# TablutBot
This repo contains an implementation of a Tablut bot using Ashton rules.
The program uses a minimax algorithm with alpha beta pruning, move ordering, and node evalutations caching using a transposition table.

## Building
The repo uses [nlohmann/json](https://github.com/nlohmann/json) for json parsing, it is installed as a submodule, so either clone the repo recursively
```bash
git clone --recursive git@github.com:SignedSnow0/TablutBot.git
```
or, if the repo is already cloned, update its submodules
```bash
git submodule init && git submodule update
```
The build system uses cmake as a dependency, simply run `make build` to build.

## Running
To run the default player simply run make with
```bash
make white
```
or
```bash
make black
```

## Logging
The program can enable logging by specifying the cmake flag `-DUSE_LOGS=On` in which case the log file will be generated in `logs/white.log` or `logs/black.log`. The logger uses the `std::format` library present in CPP20 or newer.

## Authors
- Arianna Tosello: arianna.tosello2@studio.unibo.it
- Claudio Marchini: claudio.marchni@studio.unibo.it
- Lorenzo Arlotti: lorenzo.arlotti4@studio.unibo.it
- Pierluca Pevere: pierluca.pevere@studio.unibo.it