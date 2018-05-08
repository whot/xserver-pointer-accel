#!/bin/sh
#
# Generates all dat files required. To convert everything to pngs later, run
# this command after running this script:
# export GNUTERM="png"; for g in *.gnuplot; do gnuplot -p $g > $g.png; done

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
accrange="0.02 0.03 0.04 0.05 0.06 0.07 0.08"
for acc in $accrange; do
    for min in $range; do
        for max in $range; do
            $tool $p $min $max $acc > synaptics-min$min-max$max-acc$acc.dat
        done
    done
done

# for diag comparison
$tool synaptics 0.2 0.7 0.04 > synaptics-ps2.dat
$tool synaptics 0.2 0.7 0.09 > synaptics-rmi4.dat
$tool synaptics 0.2 0.7 0.12 > synaptics-alps.dat

$libinput_tool --mode=accel --filter=linear --speed=0.0 > libinput-mouse.dat
$libinput_tool --mode=accel --filter=touchpad --speed=0.0 > libinput-touchpad.dat
