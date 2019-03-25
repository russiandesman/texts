#!/bin/bash

FILES=$*

for f in $FILES
do
  echo "Processing $f file..."
  ~/zlib/contrib/blast/blast < $f >$f.processed
done
