#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <limits>
#include <ctime>
#include <random>
#include "functions.h"
#include "structures.h"

// ------------------------ Global constants ------------------------ //

const double MAX = std::numeric_limits<double>::max();

// ------------------------ internal functions ------------------------ //

void solve_potts_subproblem(Instance* dat, LagMult& lag, Solution sol) {
    for(i=0; i<dat.n; i++) {
        for (j=i; j<dat.n; j++) {
            double ij = lag.k_sum[i][j] + lag.i_sum[i][j] + lag.j_sum[i][j] + dat->jobs[i].p*dat->jobs[j].w;
            double ji = lag.k_sum[j][i] + lag.i_sum[j][i] + lag.j_sum[j][i] + dat->jobs[j].p*dat->jobs[i].w;

            if (ij > ji && !dat->edges[i][j]) {
                lb += ji;
                sol.y[i][j] = 0;
                sol.y[j][i] = 1;
            }
            else {
                lb += ij;
                sol.y[i][j] = 1;
                sol.y[j][i] = 0;
            }
        }
    }

    lb -= 2*lag.totalSum;
}

// ------------------------ lagragian ------------------------ //

double lagrangean_relax_potts(Instance* dat) {
    // Initializing sub-grad params
    double pi = 2;
    double minPi = 0.01;
    int maxIt = 10;
    int it = 0;

    // Initializing lagragean set
    LagMult lag(dat->size);

    // Sub gradient algorithm
    double lb = -INF;

    while (pi > minPi) {
        // Getting lagragian lowerbound
        solve_potts_subproblem(dat, lag, sol);

        if (sol.lb > lb) {
            lb = sol.lb;
            it = 0;     
        }
        else {
            it += 1;

            if (it >= maxIt) {
                pi *= 0.5;

                if (pi < minPi) break;
            }
        }

        // Calculating Squared subgrad
        double sumGradSquared = lag.updadate_subGrad(sol);
        double step = pi*(sol.ub - sol.lb)/sumGradSquared;

        lag.updadate_lambdas(step);
    }

    return lb;
}