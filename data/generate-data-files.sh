#!/bin/sh

tool=$HOME/code/xserver-pointer-accel/build/ptrveloc

profiles="simple classic smooth linear polynomial power limited"
thresholds="0 2 4 6 8"
accel="0 0.5 2 4 6 8"

for p in $profiles; do
    for t in $thresholds; do
        for a in $accel; do
            $tool $p $t $a > xserver-$p-t$t-a$a.dat
        done
    done
done

