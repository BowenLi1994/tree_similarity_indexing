#!/bin/bash

COMMAND=../test/indexing_join
DATASET=syn1


for THRESHOLD in 1 5 10 100 150 200 250 300
do
    $COMMAND $DATASET 40107 $THRESHOLD &
done
