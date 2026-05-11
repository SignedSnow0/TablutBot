# README

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