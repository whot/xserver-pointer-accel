#!/usr/bin/gnuplot
# Print a comparison of the profiles with the basic defaults

set title "Comparison libinput speed vs simple profile accel"

threshold=4
accel='0 2 4 6 8'
profile='classic'


speeds="-1.0 -0.5 0.0 0.5 1.0"

filename(p, t, a) = sprintf('xserver-%s-t%d-a%s.dat', p, t, a)
filename_li(s) = sprintf('libinput-mouse.speed%s.dat', s)

label_li(v) = sprintf("speed %s", v)
label_a(v) = sprintf("accel %s", v)

set xlabel 'mm/s'
set ylabel 'factor'
set style data lines
set xrange [0:100]
set yrange [0:5]
plot \
  for [s in speeds] filename_li(s) using 1:2 title label_li(s) lt palette cb 5 + s, \
  for [a in accel] filename(profile, threshold, a) using 1:5 title label_a(a) lt palette cb 10 + a

