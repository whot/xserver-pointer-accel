#!/usr/bin/gnuplot
# Print a comparison of the profiles with the basic defaults

set title "Comparison of libinput's speed settings vs synaptics' settings"

speeds="-1.0 -0.5 0.0 0.5 1.0"

filename(mi, ma, acc) = sprintf('synaptics-min%.1f-max%.1f-acc%0.2f.dat', mi, ma, acc)
label_m(mi, ma, acc) = sprintf('%.1f/%.1f, af %.2f', mi, ma, acc)

filename_li(s) = sprintf('libinput-touchpad.speed%s.dat', s)
label_li(v) = sprintf("speed %s", v)

set xlabel 'mm/s'
set ylabel 'factor'
set style data lines
set xrange [0:100]
set yrange [0:5]

plot \
  for [s in speeds] filename_li(s) using 1:2 title label_li(s) lt palette cb 5 + s, \
  filename(0.2, 0.9, 0.12) using 1:7 title label_m(0.2, 0.8, 0.12), \
  filename(0.4, 0.7, 0.04) using 1:7 title label_m(0.4, 0.7, 0.04), \
  filename(0.6, 1.0, 0.08) using 1:7 title label_m(0.6, 1.0, 0.08),
