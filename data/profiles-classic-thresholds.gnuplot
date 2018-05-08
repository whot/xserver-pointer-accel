#!/usr/bin/gnuplot
# Print a comparison of the profiles with the basic defaults

thresholds='0 0.5 2 4 6 8'
accel=2
profile='classic'

filename(p, t, a) = sprintf('xserver-%s-t%s-a%d.dat', p, t, a)

set xlabel 'mm/s'
set ylabel 'factor'
set style data lines
set xrange [0:100]
set yrange [0:10]
plot \
  for [t in thresholds] filename(profile, t, accel) using 1:2 title t

pause -1
