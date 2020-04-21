#!/bin/bash

TOKEN="$1"
IP=""
PORT=""

printf "$TOKEN" | nc "$IP" "$PORT"

sleep 2
if ping -c 3 -w 3 "$IP"; then
  echo "couldn't suspend. host is still up."
else
  echo "suspended. host is down."
fi
