#!/bin/bash

COMMAND=../test/indexing_join
DATASET=syn2


for THRESHOLD in 1 5 10 30 50 100 150 200
do
    $COMMAND $DATASET 74320 $THRESHOLD &
done


#bolzano: 1 5 10 15 20 25 30
#sentiment: 1 5 10 15 20 25 30
#syn1: 1 5 10 30 50 150 200
#syn2: 1 50 10 30 50 100 150 200