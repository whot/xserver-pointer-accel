#!/usr/bin/gnuplot
# Print a comparison of the profiles with the basic defaults

set title "Comparison of the simple profile and libinput's profile"

threshold=4
accel=2

profiles='simple'

filename(p, t, a) = sprintf('xserver-%s-t%d-a%d.dat', p, t, a)

set xlabel 'mm/s'
set ylabel 'factor'
set style data lines
set xrange [0:100]
set yrange [0:5]
plot \
  'libinput-mouse.speed0.0.dat' using 1:2 title 'libinput mouse', \
  for [p in profiles] filename(p, threshold, accel) using 1:5 title p \

