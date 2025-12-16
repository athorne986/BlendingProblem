#include <iostream>
#include <vector>
#include <string>
#include <map>
// Include the necessary header for the CLP solver classes
#include "ClpSimplex.hpp" 
#include "OsiClpSolverInterface.hpp" // Or ClpSimplex.hpp for direct use

using namespace std;

// --- 1. DATA: Define the input parameters ---

// Global variables or constants for the problem data
const double TOTAL_BLEND = 100.0;

// Feeds and Components
const vector<string> FEEDS = {"A", "B", "C"};
const vector<string> COMPONENTS = {"X", "Y"};

// Cost of each feed ($/unit)
const map<string, double> COSTS = {
    {"A", 10.0},
    {"B", 12.0},
    {"C", 8.0}
};

// Content of component j in feed i (Fraction)
const map<string, map<string, double>> CONTENT = {
    {"A", {{"X", 0.60}, {"Y", 0.10}}},
    {"B", {{"X", 0.30}, {"Y", 0.50}}},
    {"C", {{"X", 0.20}, {"Y", 0.30}}}
};

// Minimum required content in the final blend (Fraction)
const map<string, double> REQ_MIN = {
    {"X", 0.40},
    {"Y", 0.30}
};


int main() {
    // 2. INITIALIZE THE SOLVER
    ClpSimplex model;

    // --- 3. VARIABLES: Set up columns (variables) ---
    int numVars = FEEDS.size(); // 3 variables: x_A, x_B, x_C
    
    // Set variable bounds (lower bound = 0, upper bound = infinity)
    // All variables are non-negative (x >= 0)
    double *columnLower = new double[numVars];
    double *columnUpper = new double[numVars];
    double *objective = new double[numVars];

    for (int i = 0; i < numVars; ++i) {
        string feed = FEEDS[i];
        columnLower[i] = 0.0;
        columnUpper[i] = 1.0e+20; // Represents infinity
        // Set the objective coefficients (the costs)
        objective[i] = COSTS.at(feed);
    }

    // Add variables to the model
    model.addColumns(numVars, columnLower, columnUpper, objective);
    
    // Clean up temporary arrays
    delete[] columnLower;
    delete[] columnUpper;
    delete[] objective;


    // --- 4. CONSTRAINTS: Set up rows ---
    
    // Row bounds (lower and upper limits for the constraints)
    // Row 0: Total Flow (Equality: lower = upper = 100.0)
    // Row 1: Component X (Greater than or equal: lower = 40.0, upper = infinity)
    // Row 2: Component Y (Greater than or equal: lower = 30.0, upper = infinity)
    double *rowLower = new double[3];
    double *rowUpper = new double[3];

    // Constraint 0: Total Flow (Sum(x_i) = 100.0)
    rowLower[0] = TOTAL_BLEND;
    rowUpper[0] = TOTAL_BLEND;

    // Constraint 1: Component X (Sum(x_i * C_iX) >= 0.40 * 100.0 = 40.0)
    rowLower[1] = REQ_MIN.at("X") * TOTAL_BLEND;
    rowUpper[1] = 1.0e+20; 

    // Constraint 2: Component Y (Sum(x_i * C_iY) >= 0.30 * 100.0 = 30.0)
    rowLower[2] = REQ_MIN.at("Y") * TOTAL_BLEND;
    rowUpper[2] = 1.0e+20; 
    
    // 5. Build the Constraint Matrix (A)
    // This defines the coefficients for each variable in each constraint.
    vector<int> rowIndices;
    vector<int> columnIndices;
    vector<double> elements;

    // A. Total Flow Constraint (1 * x_A + 1 * x_B + 1 * x_C)
    for (int i = 0; i < numVars; ++i) {
        rowIndices.push_back(0); // Constraint 0
        columnIndices.push_back(i); // Variable x_i
        elements.push_back(1.0); // Coefficient is 1
    }

    // B. Component Constraints (x_A*C_AX + x_B*C_BX + x_C*C_CX)
    for (int j = 0; j < COMPONENTS.size(); ++j) {
        string component = COMPONENTS[j];
        for (int i = 0; i < numVars; ++i) {
            rowIndices.push_back(j + 1); // Constraint 1 (X) or 2 (Y)
            columnIndices.push_back(i); // Variable x_i
            elements.push_back(CONTENT.at(FEEDS[i]).at(component)); 
        }
    }

    // Add rows (constraints) to the model using the built matrix
    model.addRows(3, rowLower, rowUpper, elements.size(), 
                  &rowIndices[0], &columnIndices[0], &elements[0]);

    delete[] rowLower;
    delete[] rowUpper;

    // --- 6. SOLVE THE PROBLEM ---

    // Set the problem direction: Minimize (default) or Maximize
    model.setObjSense(1.0); // 1.0 for minimization, -1.0 for maximization

    model.initialSolve(); // Find the optimal solution

    // --- 7. DISPLAY RESULTS ---
    
    if (model.isProvenOptimal()) {
        cout << "Status: Optimal" << endl;
        cout << "Minimum Total Cost: $" << model.getObjValue() << endl;

        const double *solution = model.getColSolution();

        cout << "\nOptimal Feed Quantities:" << endl;
        for (int i = 0; i < numVars; ++i) {
            cout << "  Feed " << FEEDS[i] << ": " << solution[i] << " units" << endl;
        }
    } else {
        cout << "Status: Not Optimal (" << model.status() << ")" << endl;
    }

    return 0;
}
