# Cellular Frustration Model

## Table of Contents

* [Introduction](#introduction)
* [Requirements](#requirements)
* [Usage](#usage)
* [Technologies](#technologies)
* [License](#license)

## Introduction

The Cellular Frustration Model models interactions between agents that randomly interact and bond in pairs. When normal samples are presented an unstable frustrated dynamic follows, where agents continuously change pairs, generating a recognizable normality pattern. When abnormal samples are presented this frustrated dynamic breaks down, and agents start forming lasting bonds, which signals an anomaly. These interactions mimic the ones in the immune system between T and B cells.

## Requirements

- [Python](https://www.python.org/) 3.9.5
    - [Numpy](https://numpy.org/) 1.21.0
- C++ compiler supporting the C++14 standard

## Usage

Before using this algoritm, a reading of the documentation provided in docs/ folders is strongly recommended.

**Linux/macOS**:
1. Install the GCC compiler.
2. Open a terminal inside the project directory cellular-frustration-model/
3. Provide read/write/execute permissions to the files create-defaults.sh and run.sh by typing in the terminal:
    ```bash
    chmod +rwx create-defaults.sh run.sh
    ```
4. Execute create-defaults.sh
5. Insert a data set inside the input/ folder and its accompanying files just like the examples in the data/ folder.
6. Execute run.sh
7. Check results in roc_curve.csv and auc.csv files.

## Technologies

This project was created with:
- Python 3.9.5
- C++14

## License

This project is licensed under the [MIT License](LICENSE).  
&copy; 2021 [Rodrigo Gonçalves](https://github.com/rfgon)

[Back to top](#cellular-frustration-model)
