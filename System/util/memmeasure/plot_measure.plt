reset
set terminal pdfcairo
set output 'out.pdf'
set grid
set autoscale
set ylabel 'Average copy time per tuple in ns' font ', 8'
set xlabel 'Number of attributes copied' font ', 8'
set title 'no optimization, 10^6 tuples, 4B attribute' font ',12'
set style data linespoints
set pointsize 0.5
set auto x
set yrange [0:250]
set key top left font ', 6'
plot 'data/manual.txt' using 1:2 title 'Manual Copy', 'data/normal.txt' using 1:2 title 'Memcpy', 'data/streaming.txt' using 1:2 title 'Streaming Writes'
