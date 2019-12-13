#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <ctime>
#include <queue>
#include "structures.h"
//#include "functions.h"


// ------------------- Struct Tuple ----------------------
Lambda::Lambda(int i, int j, int k, double value): i(i), j(j), k(k) value(value) {}


// ------------------- LagragMult ----------------------
LagragMult::LagragMult(int size) {
  lambdas.reserve(size * (size-1) * (size - 2));

  for (int i=0; i<size; i++) {
    for (int j=0; j<size; j++) {
      if (i == 0) continue;
      for (int k=0; k<size; k++) {
        if (k == i || k == j) continue;
        Lambda lamb(i,j,k);
        lambdas.push_back(lamb);
      }
    }
  }

  k_sum.reserve(size * size);
  j_sum.reserve(size * size);
  i_sum.reserve(size * size);
  for (int i=0; i < size; i++) {
    std::vector<double> temp(size, 0);
    k_sum.push_back(temp);
    j_sum.push_back(temp);
    i_sum.push_back(temp);
  }

  totalSum = 0;
}

double LagragMult::update_subGrad(Solution& sol) {
  double sumGradSquared = 0;

  // Updating subgrad
  for (int id=0; id<lambdas.size(); id++) {
    int i = lambdas[id].i;
    int j = lambdas[id].j;
    int k = lambdas[id].k;

    subGrad[id] = sol.y[i][j] + sol.y[j][k] + sol.y[k][i] - 2;
    subGrad += std::pow(subGrad[k], 2);
  }

  return subGrad;
}

double LagragMult::update_lambda(double step) {

  // Updating subgrad
  for (int id=0; id<lambdas.size(); id++) {
    int i = lambda[id].i;
    int j = lambda[id].j;
    int k = lambda[id].k;

    lambda[id].value = std::max(0.0, lambda[id] + step*subGrad[id]);
    i_sum[i][j] += 
  }
}

// ----------------- Struct Instance --------------------

void Instance::read_file(const char* fileName) {
  std::ifstream file(fileName);

  // checking if file exist
  if (!file.is_open()) {
    std::cout << "Instance::Instance  ";
    std::cout << "Could not open the file " << fileName << std::endl;
    std::exit(EXIT_FAILURE);
  }

  this->clear();

  file >> g >> n >> m >> i >> ub;
  size = n + m;
  jobs.reserve(size);

  edges.reserve(size);
  for (i=0; i<size; i++) {
    std::vector<bool> temp(size, false);
    edges.push_back(temp);
  }  

  // Adding Jobs
  for (int i=0; i<n; i++) {
    Job job;
    job.id = i;
    file >> job.p;
    job.w = 0;
    jobs.push_back(job);
  }

  std::string str;
  int i;
  for (int j=0; j<m; j++) {
    // Adding Orders
    Job job;
    job.id = j + n;
    file >> job.w;
    job.p = 0;
    
    std::getline(file, str);
    std::istringstream ss(str);
    while (ss >> i) {
      edges[i][j + n] = true;
    }

    jobs.push_back(job);
  }
}

void Instance::clear() {
  jobs.clear();
  std::cout << edges.size() << std::endl;
  for (int i=0; i<edges.size(); i++) {
    edges[i].clear();
  }
  edges.clear();

  n = 0;
  m = 0;
  size = 0;
  ub = INF;
}

// ----------------- Struct Solution --------------------

Solution::Solution() {
    ub = INF;
    lb = -INF;
}

Solution::Solution(Instance *inst) {
    dat = inst;
    lb = -INF;
    ub = inst->ub;

    y.reserve(dat->size);
    for (int i=0; i<dat->size; i++) {
      std::vector<bool> temp(dat->size, false);
      y.push_back(temp);
      for (int j=0; j<dat->size; j++) {
        if (inst->edges[i][j]) {
          y[i][j] = true;
        }
      }
    }    
}

void Solution::clear() {
    dat = NULL;
    ub = INF;
    lb = -INF;
    for (int i=0; i<dat->size; i++) {
      y[i].clear();
    }
    y.clear();
}
