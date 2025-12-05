#ifndef READ_INPUT_H
#define READ_INPUT_H

#include <vector>    // For using std::vector
#include <string>    // For using std::string (to read the file name)

// function for reading a 1D vector (selection criterion)
bool read_input(const std::string& filename, std::vector<double>& b);

// function for reading a 2D matrix (binary matrix)
bool read_input(const std::string& filename, std::vector<std::vector<double>>& matrix);

#endif