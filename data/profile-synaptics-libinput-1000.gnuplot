#!/usr/bin/gnuplot

mins=0.2
maxs=0.7
acc=0.04

filename_syn(mi, ma, acc) = sprintf('synaptics-min%0.1f-max%0.1f-acc%0.2f.dat', mi, ma, acc)

set xlabel 'mm/s'
set ylabel 'factor'
set style data lines
set xrange [0:1000]
set yrange [0:5]
plot \
  'libinput-touchpad.dat' using 1:2 title 'libinput touchpad', \
   filename_syn(mins, maxs, acc) using 1:7 title 'synaptics', \

