#! /bin/bash
FILE="assets.svg"
ASSETS_DIR="assets"
INDEX="assets.txt"
for i in `cat $INDEX`
do inkscape --export-id=$i --export-id-only --export-png=$ASSETS_DIR/$i.png $FILE; 
done
exit 0
