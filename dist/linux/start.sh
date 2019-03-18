#!/usr/bin/env bash

LINK_FORMAT=$(printf '\e[34;4m')
RESET_FORMAT=$(printf '\e[0m')

echo "Server addresses:"
ip -4 -o addr show scope global | awk '{gsub(/\/.*/,"",$4); print $2, "http://"$4":31415/"}' | sort | sed "s/\(http.\+\)/${LINK_FORMAT}\1${RESET_FORMAT}/"
echo ""

echo "Starting server..."
./websocketd/websocketd --binary --port 31415 ./bin/ws_handler
