#!/usr/bin/gnuplot
# Print a comparison of the profiles with the basic defaults

set title "Comparison of libinput speed settings"

speeds="-1.0 -0.5 0.0 0.5 1.0"

filename_li(s) = sprintf('libinput-mouse.speed%s.dat', s)

label_li(v) = sprintf("speed %s", v)

set xlabel 'mm/s'
set ylabel 'factor'
set style data lines
set xrange [0:100]
set yrange [0:5]

plot \
  for [s in speeds] filename_li(s) using 1:2 title label_li(s) lt palette cb 5 + s, \

