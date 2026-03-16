#!/usr/bin/env bash

function kill_bots()
{
    kill -9 $WHITE_PID
    kill -9 $BLACK_PID
}

make build
mkdir -p logs

print_white=false
print_black=false

for arg in "$@"; do
    case "$arg" in
        --print-white)
            print_white=true
            ;;
        --print-black)
            print_black=true
            ;;
        *)
            echo "Unknown option: $arg"
            exit 1
            ;;
    esac
done

if $print_white; then 
./bin/TablutBot WHITE localhost 60 | tee logs/white.log &
WHITE_PID=$!
else
./bin/TablutBot WHITE localhost 60 1>logs/white.log 2>logs/white.err &
WHITE_PID=$!
fi

if $print_black; then
./bin/TablutBot BLACK localhost 60 | tee logs/black.log &
WHITE_PID=$!
else
./bin/TablutBot BLACK localhost 60 1>logs/black.log 2>logs/black.err &
BLACK_PID=$!
fi

echo "Bots running, press ^C to stop"

trap kill_bots SIGINT
wait
