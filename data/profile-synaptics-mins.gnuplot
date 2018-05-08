#!/usr/bin/gnuplot

set title 'Effect of the MinSpeed setting'

mins=0.2
maxs=0.7
acc=0.04
vals="0.2 0.4 0.6 0.8"
profiles='synaptics'

filename(mi, ma, acc) = sprintf('synaptics-min%s-max%0.1f-acc%0.2f.dat', mi, ma, acc)

set xlabel 'mm/s'
set ylabel 'factor'
set style data lines
set xrange [0:100]
set yrange [0:10]
plot \
  for [mins in vals] filename(mins, maxs, acc) using 1:7 title mins \

