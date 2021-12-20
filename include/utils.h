#ifndef UTILS_H
#define UTILS_H

#include <iostream>     // cout, ws
#include <string>       // string, getline, substr, erase, stoi
#include <map>          // map
#include <fstream>      // ifstream
#include <algorithm>    // remove, find

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

} // namespace cfm

#endif // UTILS_H
