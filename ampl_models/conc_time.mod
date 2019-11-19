set J1;
set J2;
set A within {J1, J2};

param p {J1};
param w {J2};

var c {J2};
var y {i in J1, j in J1: i != j} binary;

minimize COST: sum{j in J2} c[j]*w[j];

s.t. r1 {(i,j) in A}: c[j] >= sum{k in J1: i!=k} p[k]*y[k,i] + p[i];
s.t. r2 {j in J2}: c[j] >= sum{i in J1: (i,j) in A} p[i];
s.t. r3 {j in J2}: c[j] <= sum{i in J1} p[i];
s.t. r4 {i in J1, j in J1: i != j}: y[i,j] + y[j,i] = 1;
s.t. r5 {i in J1, j in J1, k in J1: i != j and i != k and j != k}: y[i,j] + y[j,k] + y[k,i] <= 2;