#!/usr/bin/gnuplot

threshold=4
accel=2
profile='classic'
decels="0.25 0.5 1.0 1.5 2"

filename(p, t, a) = sprintf('xserver-%s-t%d-a%d.dat', p, t, a)

set xlabel 'mm/s'
set ylabel 'factor'
set style data lines
set xrange [0:100]
set yrange [0:10]

decel(x, c)=x * c

plot \
  for [d in decels] filename(profile, threshold, accel) using 1:(decel($5, d)) title d
