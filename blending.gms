* ----------------------------------------------------------------------
* GAMS Blending Optimization Problem (Linear Programming)
* Goal: Minimize the cost of purchasing feeds A, B, and C
* to produce a blend that meets minimum content requirements for X and Y.
* ----------------------------------------------------------------------

* 1. SETS: Define the indices used in the problem.
SET
    i Feeds / A, B, C /
    j Components / X, Y / ;

* 2. DATA (PARAMETERS): Define the input data.
* Cost of each feed stream ($/unit)
PARAMETER
    cost(i)  Cost of feed streams
    /
      A   10.0
      B   12.0
      C   8.0
    / ;

* Content of component j in feed i (Fraction, e.g., 0.6 = 60%)
TABLE
    content(i,j)
           X       Y
    A    0.60    0.10
    B    0.30    0.50
    C    0.20    0.30 ;

* Minimum required content in the final blend (Fraction)
PARAMETER
    reqMin(j) Minimum content requirement
    /
      X   0.40
      Y   0.30
    / ;

* Total amount of product to be blended (Units)
SCALAR totalBlend  Target blend quantity / 100.0 / ;


* 3. VARIABLES: Define the decision variables and the objective function.
VARIABLES
    x(i)       Quantity of feed i to purchase (Decision Variable)
    totalCost  The total cost of the blend (Objective Variable) ;

* x(i) must be non-negative.
POSITIVE VARIABLE x ;


* 4. EQUATIONS: Define the constraints and the objective function relationship.
EQUATIONS
    objFunc         Define the objective function
    totalFlow       Total quantity constraint
    compMin(j)      Minimum component content constraints ;


* 5. EQUATION DEFINITIONS: Write the algebraic expressions.
* Minimize the total cost: Sum of (Cost of feed i * Quantity of feed i)
objFunc ..
    totalCost =E= SUM(i, cost(i) * x(i)) ;

* Total quantity constraint: The sum of all feeds must equal the target blend amount.
totalFlow ..
    SUM(i, x(i)) =E= totalBlend ;

* Component minimum requirement constraint:
* The weighted average content of component j in the blend must be >= the required minimum.
compMin(j) ..
    SUM(i, content(i,j) * x(i)) =G= reqMin(j) * totalBlend ;


* 6. MODEL and SOLVE: Group the equations and solve the problem.
MODEL BlendingModel / ALL / ;

* Use the Linear Programming (LP) solver to minimize the totalCost variable.
SOLVE BlendingModel USING LP MINIMIZING totalCost ;


* 7. DISPLAY RESULTS: Output the key solution variables.
DISPLAY x.l, totalCost.l ;

* The .l suffix stands for "level" (the optimal solution value).