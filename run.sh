#!/usr/bin/env bash

TIMEOUT=10

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

if $print_white || $print_black; then
    java -jar ./lib/server/Server.jar 1>/dev/null 2>/dev/null &
else
    java -jar ./lib/server/Server.jar &
fi

sleep 1

./bin/TablutBot WHITE $TIMEOUT localhost &
WHITE_PID=$!

./bin/TablutBot BLACK $TIMEOUT localhost &
BLACK_PID=$!

trap kill_bots SIGINT

if $print_white; then 
    tail --follow ./logs/white.log
fi

if $print_black; then
    tail --follow ./logs/black.log
fi

wait
