#!/bin/bash

COMMAND=../test/tjoin_test
DATASET=syn1


for THRESHOLD in 1 5 10 50 100 150 200 
do
    $COMMAND $DATASET $THRESHOLD &
done
