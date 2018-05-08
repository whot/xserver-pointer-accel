#!/usr/bin/gnuplot

mins=0.2
maxs=0.7
threshold=4
accel=2
profiles='simple'

const_decel=1/mins # see the code...

filename_syn(mi, ma) = sprintf('synaptics-min%0.1f-max%0.1f.dat', mi, ma)
filename(p, t, a) = sprintf('xserver-%s-t%d-a%d.dat', p, t, a)

set xlabel 'mm/s'
set ylabel 'factor'
set style data lines
set xrange [0:100]
set yrange [0:10]
plot \
   filename_syn(mins, maxs) using 1:5 title 'synaptics (profile)', \
   filename_syn(mins, maxs) using 1:7 title 'synaptics (real)', \
   for [p in profiles] filename(p, threshold, accel) using 1:5 title p \

