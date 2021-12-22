#ifndef UTILS_H
#define UTILS_H

#include <iostream>     // cout, ws
#include <string>       // string, getline, substr, erase, stoi
#include <map>          // map
#include <fstream>      // ifstream
#include <algorithm>    // remove, find
#include <vector>       // vector
#include <sstream>      // stringstream

namespace cfm
{

    // Reads a file line by line and parses its contents
    std::map<std::string,int> parseParameters(std::string const file_path)
    {
        // Open file
        std::ifstream file(file_path);

        // Check if file opened correctly
        if (!file.is_open()) {
            std::cout << "Error opening file" << '\n';
            std::exit(EXIT_FAILURE);
        }

        // Parameters map
        std::map<std::string,int> params;

        // Read parameters into data structure
        while (file) {
            // Exit loop after last line is read
            if (file.eof()) {
                break;
            }

            // Get line
            std::string line;
            std::getline(file, line);

            // Get parameter's name
            std::string param_key;
            param_key = line.substr(0, line.find(':'));

            // Get parameter's value
            std::string param_val;
            param_val = line.substr(line.find(':') + 1, line.size());

            // Remove white spaces
            param_val.erase(std::remove(param_val.begin(), param_val.end(), ' '), param_val.end());

            // Insert pair into map
            params[param_key] = std::stoi(param_val);

            // Clear trailing newline
            file >> std::ws;
        }

        return params;
    }

    // Load floating-point data from a file into a matrix
    std::vector<std::vector<float>> loadFloatMatrix(std::string const file_path)
    {
        // Open file
        std::ifstream file(file_path);

        // Check if file opened correctly
        if (!file.is_open()) {
            std::cout << "Error opening file" << '\n';
            std::exit(EXIT_FAILURE);
        }

        // Data matrix
        std::vector<std::vector<float>> matrix;

        // String for line and value
        std::string line, val;

        // Read each line
        while (std::getline(file, line)) {
            // Row vector
            std::vector<float> vector;

            // Stringstream line
            std::stringstream s(line);

            // Get each feature's value
            while (std::getline(s, val, ',')) {
                // Add value to row vector
                vector.push_back(std::stod(val));
            }

            // Add row vector to matrix
            matrix.push_back(vector);
        }

        return matrix;
    }

    // Load integer data from a file into a matrix
    std::vector<std::vector<int>> loadIntMatrix(std::string const file_path)
    {
        // Open file
        std::ifstream file(file_path);

        // Check if file opened correctly
        if (!file.is_open()) {
            std::cout << "Error opening file" << '\n';
            std::exit(EXIT_FAILURE);
        }

        // Data matrix
        std::vector<std::vector<int>> matrix;

        // String for line and value
        std::string line, val;

        // Read each line
        while (std::getline(file, line)) {
            // Row vector
            std::vector<int> vector;

            // Stringstream line
            std::stringstream s(line);

            // Get each feature's value
            while (std::getline(s, val, ',')) {
                // Add value to row vector
                vector.push_back(std::stoi(val));
            }

            // Add row vector to matrix
            matrix.push_back(vector);
        }

        return matrix;
    }

    // Load unsigned integer data from a file into a matrix
    std::vector<std::vector<unsigned short int>> loadUnsignedIntMatrix(std::string const file_path)
    {
        // Open file
        std::ifstream file(file_path);

        // Check if file opened correctly
        if (!file.is_open()) {
            std::cout << "Error opening file" << '\n';
            std::exit(EXIT_FAILURE);
        }

        // Data matrix
        std::vector<std::vector<unsigned short int>> matrix;

        // String for line and value
        std::string line, val;

        // Read each line
        while (std::getline(file, line)) {
            // Row vector
            std::vector<unsigned short int> vector;

            // Stringstream line
            std::stringstream s(line);

            // Get each feature's value
            while (std::getline(s, val, ',')) {
                // Add value to row vector
                vector.push_back(std::stoi(val));
            }

            // Add row vector to matrix
            matrix.push_back(vector);
        }

        return matrix;
    }

    // Load integer data from a file into a vector
    std::vector<int> loadIntVector(std::string const file_path)
    {
        // Open file
        std::ifstream file(file_path);

        // Check if file opened correctly
        if (!file.is_open()) {
            std::cout << "Error opening file" << '\n';
            std::exit(EXIT_FAILURE);
        }

        // Data vector
        std::vector<int> vector;

        // String value
        std::string val;

        // Get each feature's value
        while (std::getline(file, val, ',')) {
            // Add value to vector
            vector.push_back(std::stoi(val));
        }

        return vector;
    }

    // Load integer data from a file into a vector
    std::vector<unsigned short int> loadUnsignedIntVector(std::string const file_path)
    {
        // Open file
        std::ifstream file(file_path);

        // Check if file opened correctly
        if (!file.is_open()) {
            std::cout << "Error opening file" << '\n';
            std::exit(EXIT_FAILURE);
        }

        // Data vector
        std::vector<unsigned short int> vector;

        // String value
        std::string val;

        // Get each feature's value
        while (std::getline(file, val, ',')) {
            // Add value to vector
            vector.push_back(std::stoi(val));
        }

        return vector;
    }

} // namespace cfm

#endif // UTILS_H
