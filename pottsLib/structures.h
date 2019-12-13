#pragma once

#include <vector>
#include <queue>
#include <limits>
#include <fstream>

// Constantes Globais
const double INF = std::numeric_limits<double>::infinity();

// Jobs
struct Job {
    int id;
    int p;
    double w;
};

struct Lambda {
    int i,j,k;
    double value;

    Lambda(int, int, int, double=0);
};

// Lagragian Multipliers
struct LagMult {
    std::vector<Lambda> lambdas;
    std::vector<Lambda> subGrad;

    std::vector<std::vector<double>> k_sum;
    std::vector<std::vector<double>> j_sum;
    std::vector<std::vector<double>> i_sum;
    double totalSum;    

    LagMult(int);
    void updadate_lambda();
}

// Instance
struct Instance {
    int g, n, m, i;
    int size;
    double ub;

    std::vector<Job> jobs;
    std::vector<std::vector<bool>> edges;

    void read_file(const char*);
    void clear();
};

// Solution 
struct Solution {
    double ub, lb;
    Instance *dat;

    std::vector<std::vector<bool>> y;

    Solution();
    Solution(Instance*);
    void clear();
};





