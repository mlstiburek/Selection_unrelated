% This script loads the source .mat files, calculates the binary matrix and b vector,
% and saves them as comma-separated text files for the C++ Gurobi optimization.

clear;
clc;
close all;

% --- Configuration ---
% Define the output directory for the text files
baseDir = 'C:\Users\Milan\OneDrive - CZU v Praze\Documents_Milan\Work\1_Papers\In_progress\Unrelated_Genetics\C_Gurobi\Unrelated_Genetics\Genetics';

% This defines the cutoff for considering two individuals to be related.
gp = 0;

% --- Load Source Data ---
% Assumes Gmat.mat (genomic relationship matrix) and Gblup.mat (selection criterion) 
% are in the MATLAB path or current directory
try
    load('Gmat.mat');
    load('Gblup.mat');
    disp('Source .mat files loaded successfully.');
catch ME
    error('Could not load Gmat.mat or Gblup.mat. Make sure they are in the MATLAB path. Error: %s', ME.message);
end

% Center the values (to match the IBD expectations)
% note that this step is data-specific or may not be needed
Gmat1 = Gmat*(0.25/0.23) + 0.01*(0.25/0.23); 

% Create the logical relationship matrix 
A_logical = Gmat1 >= gp;

% Convert the logical matrix to a numeric (double) matrix
M_new = double(A_logical);

% Create the selection response vector 'b', rounded to 2 decimal places
B_new = round(Gblup(:, 3), 2);

% --- Diagnostic Check to verify the generated matrix ---
fprintf('\n--- Diagnostic Information ---\n');
fprintf('  - Using threshold gp = %gp\n', gp);
fprintf('  - Total number of elements: %d\n', numel(M_new));
fprintf('  - Number of "related" pairs (value is 1): %d\n', nnz(M_new));
fprintf('  - Percentage of related pairs: %.2f%%\n', (nnz(M_new) / numel(M_new)) * 100);

% --- Write Output Files ---

% Define the full path for the binary matrix
fileAPath = fullfile(baseDir, 'Gbin.txt');

% This forces MATLAB to use Unix-style line endings ('\n'), which are
% guaranteed to be parsed correctly by your C++ getline function, resolving
% the invisible '\r' character issue.
writematrix(M_new, fileAPath, 'LineEnding', '\n');


% Define the full path for b.txt
filebPath = fullfile(baseDir, 'b.txt');

% Using writematrix for the 'b' vector is also cleaner and avoids line ending issues.
% By transposing B_new, it is written as a single row with comma delimiters.
%writematrix(B_new', filebPath);

fprintf('\nFiles A.txt and b.txt have been created successfully in:\n%s\n', baseDir);