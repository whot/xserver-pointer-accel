#!/usr/bin/gnuplot
# Print a comparison of the profiles with the basic defaults

set title "Comparison libinput speed vs simple profile deceleration"

threshold=4
accel=2
profile='classic'
decels="0.25 0.5 1.0 1.5 2"

speeds="-1.0 -0.5 0.0 0.5 1.0"

filename(p, t, a) = sprintf('xserver-%s-t%d-a%d.dat', p, t, a)
filename_li(s) = sprintf('libinput-mouse.speed%s.dat', s)

label_li(v) = sprintf("speed %s", v)
label_d(v) = sprintf("decel %s", v)

set xlabel 'mm/s'
set ylabel 'factor'
set style data lines
set xrange [0:100]
set yrange [0:5]

decel(x, c)=x * c

plot \
  for [s in speeds] filename_li(s) using 1:2 title label_li(s) lt palette cb 5 + s, \
  for [d in decels] filename(profile, threshold, accel) using 1:(decel($5, d)) title label_d(d) lt palette cb 10 + d

