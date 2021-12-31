#!/bin/bash

# Paths
input_path="input/"
scripts_path="scripts/"

# Generate critical files
python3 $scripts_path"gen-global-lists.py"
python3 $scripts_path"gen-critical-vals.py"
python3 $scripts_path"shuffling.py"

# Compile program
make

# Execute program
./main.out

# Clean executable files
make clean

# Calculate ROC and AUC
python3 $scripts_path"calc-roc-auc.py"
