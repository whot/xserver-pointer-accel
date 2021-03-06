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
range="0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0 1.1 1.2 1.3 1.5 2.5"
accrange="0.02 0.03 0.04 0.05 0.06 0.07 0.08 0.10 0.12 0.16"
for acc in $accrange; do
    for min in $range; do
        for max in $range; do
            $tool $p $min $max $acc > synaptics-min$min-max$max-acc$acc.dat
        done
    done
done

# for diag comparison
# >>> diag = math.hypot(5112 - 1024, 4832 - 2024)
# >>> dpi = 42 * 25.4
export SYNAPTICS_DIAGONAL="4959"
export DPI="1067"
$tool synaptics 0.4 0.7 0.04 > synaptics-ps2.dat

# >>> diag = math.hypot(1940 , 1063)
# >>> dpi = 20 * 25.4
export SYNAPTICS_DIAGONAL="2212"
export DPI="508"
$tool synaptics 0.4 0.7 0.09 > synaptics-rmi4.dat

# >>> diag = math.hypot(2000, 1400)
# >>> dpi = 25 * 25.4
# Note that this device has x res 25 and y res 32 but we can't handle this
# here
export SYNAPTICS_DIAGONAL="2441"
export DPI="635"
$tool synaptics 0.4 0.7 0.12 > synaptics-alps.dat

unset SYNAPTICS_DIAGONAL
unset DPI

speeds="-1.0 -0.5 0.0 0.5 1.0"
for s in $speeds; do
    $libinput_tool --mode=accel --filter=linear --speed=$s > libinput-mouse.speed$s.dat
    $libinput_tool --mode=accel --filter=touchpad --speed=$s > libinput-touchpad.speed$s.dat
done
