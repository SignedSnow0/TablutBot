#!/usr/bin/env bash

function kill_bots()
{
    kill -9 $WHITE_PID
    kill -9 $BLACK_PID
}

make build
mkdir -p logs

./bin/TablutBot WHITE localhost 60 &
WHITE_PID=$!

./bin/TablutBot BLACK localhost 60 1>logs/black.log 2>logs/black.err &
BLACK_PID=$!

echo "Bots running, press ^C to stop"

trap kill_bots SIGINT
wait
