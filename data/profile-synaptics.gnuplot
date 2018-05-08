#!/usr/bin/gnuplot

set title "Comparison of the simple profile and synaptics' profile"

mins=0.2
maxs=0.7
acc=0.04
threshold=4
accel=2
profiles='simple'

filename_syn(mi, ma, acc) = sprintf('synaptics-min%0.1f-max%0.1f-acc%0.2f.dat', mi, ma, acc)
filename(p, t, a) = sprintf('xserver-%s-t%d-a%d.dat', p, t, a)

set xlabel 'mm/s'
set ylabel 'factor'
set style data lines
set xrange [0:100]
set yrange [0:5]
plot \
   filename_syn(mins, maxs, acc) using 1:5 title 'synaptics (profile)', \
   filename_syn(mins, maxs, acc) using 1:7 title 'synaptics (real)', \
   for [p in profiles] filename(p, threshold, accel) using 1:5 title p \

