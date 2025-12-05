OPTIMIZATION INSTRUCTIONS
This repository contains C++ source files for performing optimization using the Gurobi Optimizer as detailed in the manuscript:

Genomic relationship-based selection of unrelated individuals

The code was developed and tested in the following environment:
•	Microsoft Visual Studio Community 2022 (64-bit) - Version 17.14.21 (November 2025)
•	Gurobi Optimizer Version 12 (Academic License)

PROJECT CONFIGURATION To compile and run these files, you must configure your Visual Studio project to link correctly with the Gurobi C++ libraries.
Please follow the official Gurobi configuration guide here: https://support.gurobi.com/hc/en-us/articles/360013194392-How-do-I-configure-a-new-Gurobi-C-project-with-Microsoft-Visual-Studio

IMPORTANT NOTE ON VERSIONING: The guide linked above may reference newer or different Gurobi versions (e.g., gurobi130). 
When following the guide, specifically in steps 9 (Additional Dependencies) and 11, you must substitute "gurobi130" with "gurobi120" to match the Gurobi 12 version used in this project. 
We also verified compatibility with Gurobi 13, which was released one week prior to our manuscript submission, and observed some performance improvements. We generally recommend using the latest version for optimal results.

DATA PREPARATION To run the optimization, two inputs (text files) must be prepared: the binary matrix derived from the genomic relationship matrix at a specific truncation point, and the vector of the selection criterion (breeding values).
We have provided a MATLAB script named "data_prep.m" to demonstrate how to generate these specific input files. 
You may use any software or approach (e.g., R, Python) to prepare the data, provided the resulting output files match the format expected by the C++ code.

FILES INCLUDED
1.	Genetics.cpp
2.	read_input.cpp
3.	read_input.h
4.	data_prep.m

CONTACT For any inquiries, please contact the corresponding author, Milan Lstiburek, at lstiburek@fld.czu.cz
