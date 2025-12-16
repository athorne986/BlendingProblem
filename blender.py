import pulp as lp

# --- 1. DATA: Define the input parameters ---

# Cost of each feed stream ($/unit)
costs = {
    'A': 10.0,
    'B': 12.0,
    'C': 8.0
}

# Content of component j in feed i (Fraction)
content = {
    'A': {'X': 0.60, 'Y': 0.10},
    'B': {'X': 0.30, 'Y': 0.50},
    'C': {'X': 0.20, 'Y': 0.30}
}

# Minimum required content in the final blend (Fraction)
req_min = {
    'X': 0.40,
    'Y': 0.30
}

# Total amount of product to be blended (Units)
total_blend = 100.0

# Define the sets for iteration
Feeds = list(costs.keys())
Components = list(req_min.keys())

# --- 2. MODEL: Create the problem instance ---

# Create the LP minimization problem
model = lp.LpProblem("Blending_Optimization_Problem", lp.LpMinimize)

# --- 3. VARIABLES: Define the decision variables ---

# Define the quantity of each feed 'x(i)' as a continuous, non-negative variable
x = lp.LpVariable.dicts("Quantity_of_Feed", Feeds, lowBound=0)

# --- 4. OBJECTIVE FUNCTION: Define the total cost ---

# Minimize the total cost: Sum of (Cost of feed i * Quantity of feed i)
# model += (objective expression, 'Name of the objective')
model += (
    lp.lpSum([costs[i] * x[i] for i in Feeds]),
    "Total_Cost"
)

# --- 5. CONSTRAINTS: Define the limits ---

# A. Total Quantity Constraint: Sum of all feeds must equal the target blend amount
model += (
    lp.lpSum([x[i] for i in Feeds]) == total_blend,
    "Total_Flow_Constraint"
)

# B. Component Minimum Requirement Constraint:
# The weighted average content of component j must be >= the required minimum.
for j in Components:
    model += (
        # Sum of (Content of j in i * Quantity of i) >= Required Min * Total Blend
        lp.lpSum([content[i][j] * x[i] for i in Feeds]) >= req_min[j] * total_blend,
        f"Min_Content_for_{j}"
    )

# --- 6. SOLVE and DISPLAY RESULTS ---

# Solve the problem (uses the default CBC solver)
model.solve()

# Check the solution status
print(f"Status: {lp.LpStatus[model.status]}")

# Display the optimal total cost
print(f"\nMinimum Total Cost: ${lp.value(model.objective):.2f}")

# Display the quantity of each feed to use
print("\nOptimal Feed Quantities:")
for i in Feeds:
    print(f"  Feed {i}: {x[i].varValue:.3f} units")
