#!/bin/bash

COMMAND=../test/tjoin_test

DATASET=bolzano
for THRESHOLD in 1 2 3 4 5
do
    $COMMAND $DATASET $THRESHOLD >> result.txt& 
done

# DATASET=sentiment
# for THRESHOLD in 1 5 10 15 20 25 30
# do
#     $COMMAND $DATASET $THRESHOLD &
# done

# DATASET=syn1
# for THRESHOLD in 1 5 10 30 50 150 200
# do
#     $COMMAND $DATASET $THRESHOLD &
# done

# DATASET=syn2
# for THRESHOLD in 1 5 10 30 50 100 150 200
# do
#     $COMMAND $DATASET $THRESHOLD &
# done

# DATASET=syn4
# for THRESHOLD in 1 5 10 30 50 100 150 200
# do
#     $COMMAND $DATASET $THRESHOLD &
# done

# DATASET=syn3
# for THRESHOLD in 1 5 10 15 20 25 30
# do
#     $COMMAND $DATASET $THRESHOLD &
# done

#bolzano: 1 5 10 15 20 25 30
#sentiment: 1 5 10 15 20 25 30
#syn1: 1 5 10 30 50 150 200
#syn2: 1 50 10 30 50 100 150 200