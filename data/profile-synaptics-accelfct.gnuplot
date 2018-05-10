#!/usr/bin/gnuplot

set title 'Effect of the maximum accel factor setting'

mins=0.2
maxs=0.7
vals="0.02 0.04 0.06 0.08"
profiles='synaptics'

filename(mi, ma, acc) = sprintf('synaptics-min%0.1f-max%.1f-acc%s.dat', mi, ma, acc)

set xlabel 'mm/s'
set ylabel 'factor'
set style data lines
set xrange [0:100]
set yrange [0:5]
plot \
  for [acc in vals] filename(mins, maxs, acc) using 1:7 title acc \

