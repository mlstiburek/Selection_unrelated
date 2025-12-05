#include "read_input.h"
#include <fstream>
#include <sstream>
#include <iostream>

// Function for reading a 1D vector
bool read_input(const std::string& filename, std::vector<double>& b) {
    std::ifstream infile(filename);
    if (!infile) {
        std::cerr << "Error opening file " << filename << "!" << std::endl;
        return false;
    }

    std::string line;
    if (!getline(infile, line)) {
        std::cerr << "Error reading data from file " << filename << "!" << std::endl;
        return false;
    }

    std::stringstream ss(line);
    double value;

    while (ss >> value) {
        b.push_back(value);
        if (ss.peek() == ',') {
            ss.ignore();
        }
    }

    return true;
}

// Function for reading a 2D matrix
bool read_input(const std::string& filename, std::vector<std::vector<double>>& matrix) {
    std::ifstream infile(filename);
    if (!infile) {
        std::cerr << "Error opening file " << filename << "!" << std::endl;
        return false;
    }

    std::string line;
    // Read the file line by line
    while (getline(infile, line)) {
        std::vector<double> row; // Create a vector to hold the row data
        std::stringstream ss(line);
        double value;

        // Read each value separated by commas
        while (ss >> value) {
            row.push_back(value);
            if (ss.peek() == ',') {
                ss.ignore();
            }
        }
        matrix.push_back(row); // Add the completed row to the matrix
    }

    return true;
}