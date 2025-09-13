#!/bin/bash
#sudo chrt --fifo 99 ./$1
#doas chrt --fifo $1 $2 $3 #<priority> <program> <cycleTime>
doas chrt --rr $1 $2 $3
