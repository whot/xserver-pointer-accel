#!/usr/bin/gnuplot

set title 'Effect of MinSpeed, MaxSpeed and AccelFactor changes'

filename(mi, ma, acc) = sprintf('synaptics-min%.1f-max%.1f-acc%0.2f.dat', mi, ma, acc)
label_m(mi, ma, acc) = sprintf('%.1f/%.1f, af %.2f', mi, ma, acc)

set xlabel 'mm/s'
set ylabel 'factor'
set style data lines
set xrange [0:100]
set yrange [0:5]
plot \
  filename(0.2, 0.9, 0.12) using 1:7 title label_m(0.2, 0.8, 0.12), \
  filename(0.4, 0.7, 0.04) using 1:7 title label_m(0.4, 0.7, 0.04), \
  filename(0.6, 1.0, 0.08) using 1:7 title label_m(0.6, 1.0, 0.08),

