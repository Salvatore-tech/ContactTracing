#!/bin/bash
gcc *.c -pthread -o Peer 
if [ $# -eq 0 ]
  then
    ./Peer
else
    for i in `seq 1 $1`; do
         gnome-terminal --tab -e ./Peer
    done
fi
exit 0

