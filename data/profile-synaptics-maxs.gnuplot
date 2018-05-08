#!/usr/bin/gnuplot

mins=0.2
maxs=0.7
vals="0.2 0.4 0.6 0.8"
profiles='synaptics'

filename(mi, ma) = sprintf('synaptics-min%0.1f-max%s.dat', mi, ma)

set xlabel 'mm/s'
set ylabel 'factor'
set style data lines
set xrange [0:100]
set yrange [0:10]
plot \
  for [maxs in vals] filename(mins, maxs) using 1:7 title maxs \

