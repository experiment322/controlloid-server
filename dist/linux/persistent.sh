#!/bin/sh
set -e
rm -f /tmp/controlloid.fifo
mkfifo /tmp/controlloid.fifo
trap "exit" INT TERM
trap "kill 0" EXIT
while true; do sleep 1d; done >/tmp/controlloid.fifo &
REMOTE_ADDR=persistent ./bin/ws_handler </tmp/controlloid.fifo >/tmp/controlloid_mappings.bin &
./websocketd/websocketd \
	--loglevel=trace --binary --port 31415 --maxforks=1 --staticdir=. \
	sh -c 'cat /tmp/controlloid_mappings.bin; cat >/tmp/controlloid.fifo'
