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

// ------------------------ Global constants ------------------------ //

const double MAX = std::numeric_limits<double>::max();

// ------------------------ internal functions ------------------------ //

void schedule_orders(Solution &sol, Instance* dat) {
    sol.ordSeq.clear();
    sol.ub = 0;
    std::vector<int> sumPred(dat->m, 0);

    // If any order does not have any predecessors
    for (int i=0; i<dat->m; i++) {
        if (dat->orders[i].sumPred == 0) {
            sol.ordSeq.push_back(i);
            sol.ordConc[i] = 0;
        }  
    }

    int t = 0;
    for (int j : sol.jobSeq) {
    t += dat->jobs[j].p;

        for (int i : dat->jobs[j].succ) {
            sumPred[i] += dat->jobs[j].p;

            if (sumPred[i] == dat->orders[i].sumPred) {
                sol.ordSeq.push_back(i);
                sol.ordConc[i] = t;
                sol.ub += t * dat->orders[i].w;
            }
        }
    }
}

void solve_subproblem (Instance* dat, std::vector<double>& lambda, Solution& sol) {
    sol.lb = 0;

    // Calculating horizon end
    int endTime = 0;
    for (int j=0; j<dat->n; j++) {
        endTime += dat->jobs[j].p;
    }

    // Calculating lambda sum
    std::vector<double> sumLambdaJob(dat->n, 0);
    std::vector<double> sumLambdaOrd(dat->m, 0);

    for (int k=0; k<dat->edges.size(); k++) {
        int i = dat->edges[k].i;
        int j = dat->edges[k].j;

        sumLambdaJob[j] += lambda[k];
        sumLambdaOrd[i] += lambda[k];
    }


    // copying sumPred
    std::vector<int> sumPred(dat->m);
    for(Order ord : dat->orders) {
        sumPred[ord.id] = ord.sumPred;
    }


    // Solving orders subproblem
    int t = 0;
    for (int i=0; i<dat->m; i++) {
        double weight = dat->orders[i].w - sumLambdaOrd[i];

        if (weight > 0) {
        sol.ordConc[i] = t + sumPred[i];
        }
        else {
        sol.ordConc[i] = endTime;
        }

        sol.lb += sol.ordConc[i] * weight;
    }

    // Solving jobs subproblem
    std::vector<double> priority;
    std::vector<int> idx;

    for (int j=0; j<dat->n; j++) {
        priority.push_back(dat->jobs[j].p / sumLambdaJob[j]);
        idx.push_back(j);
    }

    std::vector<int> sorted(idx.size());
    std::iota(sorted.begin(), sorted.end(), 0);
    std::sort(sorted.begin(), sorted.end(), [&](int a, int b){return priority[a] < priority[b]; });

    t = 0;
    for (int count=0; count<idx.size(); count++) {
        int j = idx[sorted[count]];

        sol.jobSeq.push_back(j);
        t += dat->jobs[j].p;
        sol.jobConc[j] = t;
        sol.lb += t * sumLambdaJob[j];
    }
}


// ------------------------ heuristics ------------------------ //

Solution smith_naive (Instance* dat) {
	Solution sol(dat);

	// calculating smith ration
	std::vector<double> smithRatio(dat->m);
	for (int i=0; i<dat->m; i++) {
		smithRatio[i] = dat->orders[i].sumPred / dat->orders[i].w;
	}

	// sorting orders id by smith ratio
    auto comp = [&smithRatio] (int i, int j) {
		return smithRatio[i] < smithRatio[j];
	};
    sol.ordSeq.resize(dat->m);
	std::iota(sol.ordSeq.begin(), sol.ordSeq.end(), 0);
	std::sort(sol.ordSeq.begin(), sol.ordSeq.end(), comp);

    // for (int i=0; i<dat->m; i++) {
    //     std::cout << sol.ordSeq[i] << " ";
    // }
    // std::cout << std::endl;

    // schedulng J1 jobs
    std::vector<bool> scheduled(dat->n, false);
    int t = 0;
    sol.ub = 0;
    for (int i : sol.ordSeq) {
        // std::cout << i << " -> ";
        for (int j : dat->orders[i].pred) {
            if (!scheduled[j]) {
                // std::cout << j << " ";
                sol.jobSeq.push_back(j);
                t += dat->jobs[j].p;
                scheduled[j] = true;
            }
        }
        // std::cout << "END: " << t << std::endl;
        sol.ub += t*dat->orders[i].w;
        sol.ordConc[i] = t;
    }

    return sol;
}

Solution smith_naive_plus (Instance* dat) {
    Solution sol(dat);

	// calculating smith ration
	std::vector<double> smithRatio(dat->m);
	for (int i=0; i<dat->m; i++) {
		smithRatio[i] = dat->orders[i].sumPred / dat->orders[i].w;
	}

	// sorting orders id by smith ratio
    auto comp = [&smithRatio] (int i, int j) {
		return smithRatio[i] < smithRatio[j];
	};
    sol.ordSeq.resize(dat->m);
	std::iota(sol.ordSeq.begin(), sol.ordSeq.end(), 0);
	std::sort(sol.ordSeq.begin(), sol.ordSeq.end(), comp);

    std::vector<bool> scheduled(dat->n, false);
    for (int i : sol.ordSeq) {
        // std::cout << i << " -> ";
        for (int j : dat->orders[i].pred) {
            if (!scheduled[j]) {
                // std::cout << j << " ";
                sol.jobSeq.push_back(j);
                scheduled[j] = true;
            }
        }
        // std::cout << "END: " << t << std::endl;
    }

    schedule_orders(sol, dat);
    return sol;
}

Solution smith_dinamic (Instance* dat) {
    Solution sol(dat);

    // copying sumPred
	std::vector<int> sumPred(dat->m);
	for (Order ord : dat->orders) {
		sumPred[ord.id] = ord.sumPred;
	}

    std::vector<bool> ordScheduled(dat->n, false);
    std::vector<bool> jobScheduled(dat->n, false);
    int t = 0;
    sol.ub = 0;
    for (int k=0; k<dat->m; k++) {
        // choosing best order to target
        double minRatio = MAX;
        double target;
        for (int i=0; i<dat->m; i++) {
            if (ordScheduled[i]) continue;

            double smithRatio = sumPred[i] / dat->orders[i].w;
            if (smithRatio < minRatio) {
                minRatio = smithRatio;
                target = i;
            }
        }
        

        // scheduling preds of the target
        for (int j : dat->orders[target].pred) {
            if (jobScheduled[j]) continue;

            sol.jobSeq.push_back(j);
            jobScheduled[j] = true;
            t += dat->jobs[j].p;
            sol.jobConc[j] = t;
            
            for (int i : dat->jobs[j].succ) {
                //if (ordScheduled[i]) continue; 

                sumPred[i] -= dat->jobs[j].p;

                if (sumPred[i] == 0) {
                    ordScheduled[i] = true;
                    sol.ordSeq.push_back(i);
                    sol.ordConc[i] = t;
                    sol.ub += t*dat->orders[i].w;
                }
            }
        }
    }

    return sol;
}

Solution priority_scheduling (Instance* dat) {
	Solution sol(dat);
	std::vector<bool> scheduled(dat->n, false);
	int t = 0;

	// copying sumPred
	std::vector<int> sumPred(dat->m);
	for (Order ord : dat->orders) {
		sumPred[ord.id] = ord.sumPred;
	}

	for (int count=0; count<dat->n; count++) {
		double alphaMax = 0;
		int bestJob = -1;

		for (int j = 0; j < dat->n; j++) {
			if (scheduled[j]) continue;
				double alpha = 0;

			for (int i : dat->jobs[j].succ) {
				alpha += dat->orders[i].w / sumPred[i];
			}

			if (alpha >= alphaMax) {
				alphaMax = alpha;
				bestJob = j;
			}
		}

		// Scheduling best option
		sol.jobSeq.push_back(bestJob);
		t += dat->jobs[bestJob].p;
		sol.jobConc[bestJob] = t;
		scheduled[bestJob] = true;

		// Updating sumPred
		for (int i : dat->jobs[bestJob].succ) {
			sumPred[i] -= dat->jobs[bestJob].p;
		}
	}

	schedule_orders(sol, dat);
	return sol;
}

Solution assemble_heuristic (Instance* dat) {
    Solution sol(dat);

    // copying sumPred
	std::vector<int> sumPred(dat->m);
	for (Order ord : dat->orders) {
		sumPred[ord.id] = ord.sumPred;
	}

    std::vector<bool> ordScheduled(dat->n, false);
    std::vector<bool> jobScheduled(dat->n, false);
    int t = 0;
    sol.ub = 0;
    for (int k=0; k<dat->m; k++) {
        // choosing best order to target
        double minRatio = MAX;
        double target;
        for (int i=0; i<dat->m; i++) {
            if (ordScheduled[i]) continue;

            double smithRatio = sumPred[i] / dat->orders[i].w;
            if (smithRatio < minRatio) {
                minRatio = smithRatio;
                target = i;
            }
        }

        // scheduling preds of the target with prioryty scheduling
        while(true) {
            double alphaMax = 0;
            int bestJob = -1;

            for (int j : dat->orders[target].pred) {
                if (jobScheduled[j]) continue;
                
                double alpha = 0;
                for (int i : dat->jobs[j].succ) {
                    if (ordScheduled[i]) continue;
                    alpha += dat->orders[i].w / sumPred[i];
                }

                if (alpha >= alphaMax) {
                    alphaMax = alpha;
                    bestJob = j;
                }
            }

            if(bestJob == -1) break;

            // Scheduling best option
            sol.jobSeq.push_back(bestJob);
            jobScheduled[bestJob] = true;
            t += dat->jobs[bestJob].p;
            sol.jobConc[bestJob] = t;

            // Updating sumPred
            for (int i : dat->jobs[bestJob].succ) {
                sumPred[i] -= dat->jobs[bestJob].p;
                
                if (sumPred[i] == 0) {
                    ordScheduled[i] = true;
                    sol.ordSeq.push_back(i);
                    sol.ordConc[i] = t;
                    sol.ub += t*dat->orders[i].w;
                }
            }
        }

        // ordScheduled[target] = true;
        // sol.ordSeq.push_back(target);
        // sol.ordConc[target] = t;
        // sol.ub += t*dat->orders[target].w;  
    }

    return sol;    
}

// ------------------------ lagragian ------------------------ //
double lagrangean_relax(Instance* dat) {
    // Initializing sub-grad params
    double pi = 2;
    double minPi = 0.01;
    int maxIt = 10;
    int it = 0;

    // Initializing lagragean multipliers
    std::vector<double> lambda(dat->edges.size(), 0);

    // Initiazling subgradients
    std::vector<double> subGrad(dat->edges.size(), 0);

    // Sub gradient algorithm
    Solution sol = assemble_heuristic(dat);
    double lb = -INF;

    while (pi > minPi) {
        double sumGradSquared = 0;

        // Getting lagragian lowerbound
        solve_subproblem(dat, lambda, sol);

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

        // Updating subgradient
        for (int k=0; k<dat->edges.size(); k++) {
            int i = dat->edges[k].i;
            int j = dat->edges[k].j;

            subGrad[k] = sol.jobConc[j] - sol.ordConc[i];
            sumGradSquared += std::pow(subGrad[k], 2);
        }

        double step = pi*(sol.ub - sol.lb)/sumGradSquared;

        for (int k=0; k<dat->edges.size(); k++) {
            lambda[k] = std::max(0.0, lambda[k] + step*subGrad[k]);
        }
    }

    return lb;
}


// // ------------------------------- FUNCTIONS -------------------------------

// // Heuristicas Proposta
// Solution jobs_scheduling_heuristic(Instance* dat, std::vector<int>& fixed) {
//   Solution sol(dat);
//   std::vector<bool> scheduled(dat->n, false);
//   int t = 0;

//   // copying sumPred
//   std::vector<int> sumPred(dat->m);
//   for (Order ord : dat->orders) {
//     sumPred[ord.id] = ord.sumPred;
//   }

//   // Adding fixed terms
//   for (int j : fixed) {
//     sol.jogSeq.push_back(j);
//     t += dat->jobs[j].p;
//     sol.jobConc[j] = t;
//     scheduled[j] = true;

//     for (int i : dat->jobs[j].succ) {
//       sumPred[i] -= dat->jobs[j].p;
//     }
//   }  

//   for (int count = fixed.size(); count < dat->n; count++) {
//     double alphaMax = 0;
//     int bestJob = -1;

//     for (int j = 0; j < dat->n; j++) {
//       if (scheduled[j]) continue;
//       double alpha = 0;

//       for (int i : dat->jobs[j].succ) {
//         alpha += dat->orders[i].w / sumPred[i];
//       }

//       if (alpha >= alphaMax) {
//         alphaMax = alpha;
//         bestJob = j;
//       }
//     }

//     // Scheduling best option
//     sol.jobSeq.push_back(bestJob);
//     t += dat->jobs[bestJob].p;
//     sol.jobConc[bestJob] = t;
//     scheduled[bestJob] = true;

//     // Updating sumPred
//     for (int i : dat->jobs[bestJob].succ) {
//       sumPred[i] -= dat->jobs[bestJob].p;
//     }
//   }

//   schedule_orders(sol, dat);
//   return sol;
// }


// // schedule orders based jobs schedule
// void schedule_orders(Solution &sol, Instance* dat) {
//   sol.ordSeq.clear();
//   sol.ub = 0;
//   std::vector<int> sumPred(dat->m, 0);

//   // If any order does not have any predecessors
//   for (int i=0; i<dat->m; i++) {
//     if (dat->orders[i].sumPred == 0) {
//       sol.ordSeq.push_back(i);
//       sol.ordConc[i] = 0;
//     }  
//   }

//   int t = 0;
//   for (int j : sol.jobSeq) {
//     t += dat->jobs[j].p;

//     for (int i : dat->jobs[j].succ) {
//       sumPred[i] += dat->jobs[j].p;

//       if (sumPred[i] == dat->orders[i].sumPred) {
//         sol.ordSeq.push_back(i);
//         sol.ordConc[i] = t;
//         sol.ub += t * dat->orders[i].w;
//       }
//     }
//   }
// }

