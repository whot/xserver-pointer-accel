#!/usr/bin/gnuplot
# Print a comparison of the profiles with the basic defaults

threshold=4
accel=2

profiles='classic simple smooth linear'

filename(p, t, a) = sprintf('xserver-%s-t%d-a%d.dat', p, t, a)

set xlabel 'mm/s'
set ylabel 'factor'
set style data lines
# set xrange [0:1000]
set yrange [0:10]
plot \
  for [p in profiles] filename(p, threshold, accel) using 1:2 title p \

pause -1
