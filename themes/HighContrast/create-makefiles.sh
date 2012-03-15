#!/bin/bash

FILE=Makefile-icons.am

echo "NULL=" > $FILE
echo "EXTRA_DIST = \\" >> $FILE
find . \( -name "*.png" -or -name "*.svg" \) -print | awk '{ print $$0, " \\" }' >> $FILE
echo "\$(NULL)" >> $FILE
