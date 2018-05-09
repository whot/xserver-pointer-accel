#!/usr/bin/gnuplot
# Print a comparison of the profiles with the basic defaults

set title "Comparison libinput speed vs simple profile threshold"

thresholds='0 0.5 2 4 6 8'
accel=2
profile='classic'


speeds="-1.0 -0.5 0.0 0.5 1.0"

filename(p, t, a) = sprintf('xserver-%s-t%s-a%d.dat', p, t, a)
filename_li(s) = sprintf('libinput-mouse.speed%s.dat', s)

label_li(v) = sprintf("speed %s", v)
label_t(v) = sprintf("threshold %s", v)

set xlabel 'mm/s'
set ylabel 'factor'
set style data lines
set xrange [0:100]
set yrange [0:5]
plot \
  for [s in speeds] filename_li(s) using 1:2 title label_li(s) lt palette cb 5 + s, \
  for [t in thresholds] filename(profile, t, accel) using 1:5 title label_t(t) lt palette cb 8 + t

