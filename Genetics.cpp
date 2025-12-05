#include "gurobi_c++.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
#include <string>
#include <limits> // std::numeric_limits
#include <cmath>  // round
#include "read_input.h"

using namespace std;

// function for saving the result
void save_solution(int nc, const vector<GRBVar>& r, const GRBModel& model) {
    string filename = "solution_nc_" + to_string(nc) + ".txt";
    ofstream result_file(filename);
    if (!result_file) {
        cerr << "ERROR: Could not open file for writing: " << filename << endl;
        return;
    }
    result_file << "Best solution for nc = " << nc << " within time limit:" << endl;
    result_file << "Incumbent (Best Objective): " << model.get(GRB_DoubleAttr_ObjVal) << endl;
    result_file << "Best Bound: " << model.get(GRB_DoubleAttr_ObjBound) << endl;
    result_file << "Average Response: " << model.get(GRB_DoubleAttr_ObjVal) / nc << endl;
    result_file << "Final Optimality Gap: " << model.get(GRB_DoubleAttr_MIPGap) * 100 << " %" << endl;
    result_file << "Solve Time (s): " << model.get(GRB_DoubleAttr_Runtime) << endl;

    result_file << "\nSelected individuals (r[i] = 1):\n";
    for (int i = 0; i < r.size(); ++i) {
        if (r[i].get(GRB_DoubleAttr_X) > 0.5) {
            result_file << "r[" << i << "]" << endl;
        }
    }
    result_file.close();
    cout << "Solution for nc = " << nc << " successfully saved to " << filename << endl;
}

int main(int argc, char* argv[]) {
    try {
        GRBEnv env = GRBEnv(true);
        env.set("OutputFlag", "1");
        env.start();

        int n = 1792;
        int nc_max = 0;

        // --- Reading data ---
        vector<double> b;
        if (!read_input("b.txt", b)) { return 1; }
        vector<vector<double>> Gbin;
        if (!read_input("Gbin.txt", Gbin)) { return 1; }
        vector<double> SumAboveDiagonal(n, 0.0);
        for (int i = 0; i < n; ++i) { for (int j = i + 1; j < n; ++j) { SumAboveDiagonal[i] += Gbin[i][j]; } }
        cout << "Data loaded successfully" << endl;
        // --- End of data reading ---

        vector<double> previous_solution(n, 0.0);

        // ====================================================================
        // Phase 1: Solve the first model to determine nc_max
        // ====================================================================
        cout << "\n--- Obtaining starting solution and nc_max from the first model ---" << endl;
        {
            GRBModel model_nc = GRBModel(env);
            vector<GRBVar> r_nc(n);
            for (int i = 0; i < n; i++) { r_nc[i] = model_nc.addVar(0.0, 1.0, 0.0, GRB_BINARY); }

            GRBLinExpr obj_nc = 0;
            for (int i = 0; i < n; i++) { obj_nc += r_nc[i]; }
            model_nc.setObjective(obj_nc, GRB_MAXIMIZE);

            for (int i = 0; i < n; ++i) {
                if (SumAboveDiagonal[i] > 0) {
                    GRBLinExpr lhs = 0;
                    lhs += SumAboveDiagonal[i] * r_nc[i];
                    for (int j = i + 1; j < n; ++j) { if (Gbin[i][j] == 1) { lhs += r_nc[j]; } }
                    model_nc.addConstr(lhs <= SumAboveDiagonal[i]);
                }
            }

            model_nc.set(GRB_IntParam_MIPFocus, 2);
            // model_nc.set(GRB_DoubleParam_TimeLimit, 3600.0);
            model_nc.optimize();

            if (model_nc.get(GRB_IntAttr_Status) == GRB_OPTIMAL) {
                nc_max = static_cast<int>(round(model_nc.get(GRB_DoubleAttr_ObjVal)));
                cout << "Found maximum value nc_max = " << nc_max << endl;

                for (int i = 0; i < n; i++) { previous_solution[i] = r_nc[i].get(GRB_DoubleAttr_X); }
            }
            else {
                cerr << "Failed to find a solution for nc_max; cannot continue." << endl;
                return 1;
            }
        }

        // ====================================================================
        // Phase 2: Solving instances for nc = nc_max, nc_max-1, ..., 1 with "warm start"
        // ====================================================================
        for (int current_nc = nc_max; current_nc >= 1; --current_nc) {
            cout << "\n--------------------------------------------------" << endl;
            cout << "--- Starting optimization for nc = " << current_nc << " ---" << endl;
            cout << "--------------------------------------------------" << endl;

            GRBModel model = GRBModel(env);
            vector<GRBVar> r(n);
            for (int i = 0; i < n; i++) { r[i] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY); }

            // Setting warm start
            vector<double> feasible_start = previous_solution;
            int current_sum = 0;
            for (double val : feasible_start) { if (val > 0.5) current_sum++; }

            while (current_sum > current_nc) {
                double min_b_in_solution = numeric_limits<double>::max();
                int index_to_remove = -1;
                for (int i = 0; i < n; ++i) {
                    if (feasible_start[i] > 0.5 && b[i] < min_b_in_solution) {
                        min_b_in_solution = b[i];
                        index_to_remove = i;
                    }
                }
                if (index_to_remove != -1) {
                    feasible_start[index_to_remove] = 0.0;
                    current_sum--;
                }
                else { break; }
            }

            for (int i = 0; i < n; i++) { r[i].set(GRB_DoubleAttr_Start, feasible_start[i]); }

            GRBLinExpr obj = 0;
            for (int i = 0; i < n; i++) { obj += r[i] * b[i]; }
            model.setObjective(obj, GRB_MAXIMIZE);

            GRBLinExpr sum_of_r = 0;
            for (int i = 0; i < n; i++) { sum_of_r += r[i]; }
            model.addConstr(sum_of_r == current_nc, "c0");

            for (int i = 0; i < n; ++i) {
                if (SumAboveDiagonal[i] > 0) {
                    GRBLinExpr lhs = 0;
                    lhs += SumAboveDiagonal[i] * r[i];
                    for (int j = i + 1; j < n; ++j) { if (Gbin[i][j] == 1) { lhs += r[j]; } }
                    model.addConstr(lhs <= SumAboveDiagonal[i]);
                }
            }

            // Applying best parameters from the tuning phase
            model.set(GRB_IntParam_MIPFocus, 3);
            model.set(GRB_IntParam_RINS, 2500);

            // <<< Time limit >>>
            cout << "Info: Setting time limit to 3 hours (10800s)." << endl;
            model.set(GRB_DoubleParam_TimeLimit, 18000.0);

            model.optimize();

            if (model.get(GRB_IntAttr_Status) == GRB_OPTIMAL || model.get(GRB_IntAttr_Status) == GRB_TIME_LIMIT) {
                if (model.get(GRB_IntAttr_SolCount) > 0) {
                    save_solution(current_nc, r, model);
                    for (int i = 0; i < n; i++) { previous_solution[i] = r[i].get(GRB_DoubleAttr_X); }
                }
                else {
                    cout << "Failed to find a solution for nc = " << current_nc << " within the time limit." << endl;
                }
            }
            else {
                cout << "Failed to find a solution for nc = " << current_nc << endl;
            }
        }

    }
    catch (GRBException& e) {
        cout << "Error code = " << e.getErrorCode() << endl;
        cout << e.getMessage() << endl;
    }
    catch (...) {
        cout << "Exception during optimization!" << endl;
    }

    return 0;
}