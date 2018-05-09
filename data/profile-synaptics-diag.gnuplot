#!/usr/bin/gnuplot

set title 'Effect of different min/max ranges'

mins=0.4
maxs=0.7
acc=0.04

filename_syn(mi, ma, acc) = sprintf('synaptics-min%0.1f-max%0.1f-acc%0.2f.dat', mi, ma, acc)

set xlabel 'mm/s'
set ylabel 'factor'
set style data lines
set xrange [0:100]
set yrange [0:5]
plot \
  'synaptics-ps2.dat' using 1:7 title 'ps/2', \
  'synaptics-rmi4.dat' using 1:7 title 'rmi4', \
  'synaptics-alps.dat' using 1:7 title 'alps', \

