#!/bin/bash

j=0
for i in *.pdf; do
    j=$((j + 1))
    out='test-'$(printf "%03d" $j)'.png'
    convert -density 300 -trim -resize 600x456 -background White -gravity center -extent 600x456  $i  $out
    out='test-'$(printf "%03d" $j)'.eps'
    pdfcrop $i cropped.pdf
    pdftops -eps cropped.pdf $out
    rm cropped.pdf
done
ffmpeg -framerate 5 -i test-%03d.png -c:v libx264 -r 30 -pix_fmt yuv420p out.mp4
