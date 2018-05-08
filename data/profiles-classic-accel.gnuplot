#!/usr/bin/gnuplot
# Print a comparison of the profiles with the basic defaults

threshold=4
accel='0 2 4 6 8'
profile='classic'

filename(p, t, a) = sprintf('xserver-%s-t%d-a%s.dat', p, t, a)

set xlabel 'mm/s'
set ylabel 'factor'
set style data lines
set xrange [0:100]
set yrange [0:10]
plot \
  for [a in accel] filename(profile, threshold, a) using 1:2 title a

pause -1
