#!/bin/bash

TOKEN="$1"
IP=""
PORT=""

printf "$TOKEN" | nc "$IP" "$PORT"

sleep 2
ping -c 3 -w 3 "$IP"
