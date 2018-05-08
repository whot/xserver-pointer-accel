#!/bin/sh

tool=$HOME/code/xserver-pointer-accel/build/ptrveloc
libinput_tool=$HOME/code/libinput/build/ptraccel-debug

profiles="simple classic smooth linear polynomial power limited"
thresholds="0 0.5 2 4 6 8"
accel="0 0.5 2 4 6 8"

for p in $profiles; do
    for t in $thresholds; do
        for a in $accel; do
            $tool $p $t $a > xserver-$p-t$t-a$a.dat
        done
    done
done


p=synaptics
range="0.2 0.3 0.4 0.5 0.6 0.7 0.8"
for min in $range; do
    for max in $range; do
        $tool $p $min $max > synaptics-min$min-max$max.dat
    done
done

$libinput_tool --mode=accel --filter=linear --speed=0.0 > libinput-mouse.dat
$libinput_tool --mode=accel --filter=touchpad --speed=0.0 > libinput-touchpad.dat
