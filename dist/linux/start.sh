#!/usr/bin/env bash

echo "Server addresses:"
ip -4 -o addr show scope global | awk '{gsub(/\/.*/,"",$4); print $2, "http://"$4":31415/"}'
echo

echo "Starting server..."
./websocketd/websocketd --binary --port 31415 ./bin/ws_handler
