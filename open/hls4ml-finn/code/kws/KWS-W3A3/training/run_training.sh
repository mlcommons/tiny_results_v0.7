#!/usr/bin/env bash

# Start the experment
echo Starting KWS training as experiment
OUTPUT=$(det experiment create --template kws_tf const_QMLP.yaml . | grep "Created experiment")

# Save the experment number
expNumber="${OUTPUT//'Created experiment '/''}"
echo Started experiment with number $expNumber
echo Saving experiment number to experiment_number.txt
echo $expNumber > experiment_number.txt
