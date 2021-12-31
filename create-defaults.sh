#!/bin/bash

# Paths
input_path="input/"

# Create default folders
mkdir -p input output

# Create default parameters file
printf "seed: 0\nsample rounds: 100\nfrustration rounds: 100000\ntrain: 1\ntraining rounds: 1000000\ntraining interval: 1500\nmonitor: 1\nmonitoring rounds: 1000\npresenters sets: 10\nmax nu: 0.2\nactivation threshold percent: 5\n" > $input_path"parameters.txt"
