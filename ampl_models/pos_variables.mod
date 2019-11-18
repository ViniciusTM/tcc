set J1;
set J2;
set J := J1 union J2;
set A within {J1, J2};

param p {J1};
param w {J2};

var y {i in J, j in J: i != j} binary;

minimize COST: sum{(i,j) in A} p[i]*w[j]*y[i,j];

s.t. r1 {(i,j) in A}: y[i,j] = 1;
s.t. r2 {i in J, j in J: i != j}: y[i,j] + y[j,i] = 1;
s.t. r3 {i in J, j in J, k in J: i != j and i != k and j != k}: y[i,j] + y[j,k] + y[k,i] <= 2;