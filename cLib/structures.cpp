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
#include "functions.h"


// ------------------- Struct Edges ----------------------

Edge::Edge(int i, int j): i(i), j(j) {}


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

  file >> g >> n >> m >> i;
  jobs.reserve(n);
  orders.reserve(m);
  edges.reserve(n*m);

  // Adding Jobs
  for (int j=0; j<n; j++) {
    Job job;
    job.id = j;
    file >> job.p;
    job.succ.reserve(m);

    jobs.push_back(job);
  }

  std::string str;
  int j;
  for (int i=0; i<m; i++) {
    // Adding Orders
    Order ord;
    ord.id = i;
    ord.sumPred = 0;
    file >> ord.w;

    ord.pred.reserve(n);
    
    std::getline(file, str);
    std::istringstream ss(str);
    while (ss >> j) {
      edges.push_back(Edge(i, j));
      jobs[j].succ.push_back(i);
      ord.pred.push_back(j);
      ord.sumPred += jobs[j].p; 
    }

    orders.push_back(ord);
  }
}

void Instance::clear() {
  n = 0;
  m = 0;

  jobs.clear();
  orders.clear();
  edges.clear();
}

// ----------------- Struct Solution --------------------

Solution::Solution() {
    ub = INF;
    lb = -INF;
}

Solution::Solution(Instance *inst) {
    this->link_data(inst);
    dat = inst;
}

void Solution::link_data(Instance *inst) {
    this->clear();
    jobSeq.reserve(inst->n);
    ordSeq.reserve(inst->m);

    jobConc.resize(inst->n);
    ordConc.resize(inst->m);
}

void Solution::clear() {
    dat = NULL;
    ub = INF;
    lb = -INF;
    jobSeq.clear();
    ordSeq.clear();
    jobConc.clear();
    ordConc.clear();
}

void Solution::copy_to(Solution& sol) {
  sol.ub = ub;
  sol.lb = lb;

  sol.jobSeq.resize(jobSeq.size());
  for (int j=0; j<jobSeq.size(); j++) {
    sol.jobSeq[j] = jobSeq[j];
  }

  sol.ordSeq.resize(ordSeq.size());
  for (int i=0; i<ordSeq.size(); i++) {
    sol.ordSeq[i] = ordSeq[i];
  }

  sol.jobConc.resize(jobConc.size());
  for (int j=0; j<jobConc.size(); j++) {
    sol.jobConc[j] = jobConc[j];
  }

  sol.ordConc.resize(ordConc.size());
  for (int i=0; i<ordConc.size(); i++) {
    sol.ordConc[i] = ordConc[i];
  }
}

void Solution::print_sol() {
  std::cout << "OF: " << ub << std::endl;
  
  std::cout << "Jobs Sequence: ";
  for (int j=0; j<jobSeq.size(); j++) {
    std::cout << jobSeq[j] << "(" << jobConc[jobSeq[j]] << ") ";
  }
  std::cout << std::endl;
  std::cout << std::endl;

  std::cout << "Orders Sequence: ";
  for (int i=0; i<ordConc.size(); i++) {
    if (ordSeq.size() > 0)
      std::cout << ordSeq[i] << "(" << ordConc[ordSeq[i]] << ") ";
    else
      std::cout << "(" << ordConc[i] << ") ";
  }
  std::cout << std::endl;
}

// ----------------- Struct Node --------------------

// Node::Node() {
//   ub = INF;
//   lb = -INF;
//   depth = 0;
//   parent = nullptr;
// }

// Node::Node(Node *father, Instance* dat, int fix) {
//   ub = INF;
//   lb = -INF;
//   this->link_data(dat);

//   depth = father->depth + 1;
//   fixed = father->fixed;

//   fixed.push_back(fix);
// }

// void Node::link_data(Instance* dat) {
//   sol.link_data(dat);

//   lagMult.resize(dat->edges.size());
//   for (int k=0; k<dat->edges.size(); k++) {
//     lagMult[k] = 1;
//   }
// }

// std::vector<Node> Node::make_children(Instance* dat) {
//   std::vector<Node> children;

//   for (int j=0; j<dat->n; j++) {
//     if(std::find(fixed.begin(), fixed.end(), j) == fixed.end()) {
//       Node c(this, dat, j);
//       children.push_back(c);
//     }
//   }

//   return children;
// }

// void Node::write_results(std::ofstream& file, Instance *dat) {


//   file << dat->n << ',' << dat->m << ',' << dat->i << ',' << time << ',' << ub << ',' << lb << ',' << gap << std::endl;
// }

// void Node::write_log(std::ofstream& file, Instance *dat) {
//   file << dat->n << ',' << dat->m << ',' << dat->i << std::endl;

//   for (int k=0; k<dat->edges.size(); k++) {
//     int i = dat->edges[k].i;
//     int j = dat->edges[k].j;

//     file << i << ',' << j << ',' << lagMult[k] << std::endl;
//   }
//   file << "end" << std::endl;
// }

// bool Node::operator<(const Node& rhs) const {

//   return lb > rhs.lb;
// }

// // ----------------- Struct Solver ------------------

// Solver::Solver(Instance *inst) {
//   dat = inst;
//   root.link_data(dat);
//   calc_bounds(root);
//   ub = root.ub;
//   lb = root.lb;
//   queue.push(root);
// }

// Solution Solver::solve() {
//   while (queue.size() > 0) {
//     Node node = queue.top();
//     queue.pop();

//     for (Node children : node.make_children(dat)) {
//       if (calc_bounds(children)) return children.sol;
//       queue.push(children);
//     }
//   }

//   return Solution(dat);
// }

// bool Solver::calc_bounds(Node& node) {
//   clock_t start = clock();

//   jobs_scheduling_heuristic(dat).copy_to(node.sol);
//   node.ub = node.sol.ub;
//   std::vector<double> subGrad(dat->edges.size(), 0);
//   Solution currentSol;

//   double pi = 2;
//   double minPi = 0.01;
//   int maxIt = 10;
//   int it = 0;

//   // Sub gradient algorithm
//   while (pi > minPi) {
//     double sumGradSquared = 0;

//     // Getting lagragian lowerbound
//     currentSol = solve_subproblem(&node, dat);
//     if (currentSol.lb > node.lb) {
//       node.lb = currentSol.lb;
//       it = 0;

//       if (std::abs(node.ub - node.lb)/(1 + std::abs(node.lb)) <= 0.0001) {
//         node.time =  static_cast<double>((clock() - start)/CLOCKS_PER_SEC);
//         return true;
//       }
//     }
//     else {
//       it += 1;

//       if (it >= maxIt) {
//         pi *= 0.5;

//         if (pi < minPi) break;
//       }
//     }

//     // Updating subgradient
//     for (int k=0; k<dat->edges.size(); k++) {
//       int i = dat->edges[k].i;
//       int j = dat->edges[k].j;

//       subGrad[k] = currentSol.jobConc[j] - currentSol.ordConc[i];
//       sumGradSquared += std::pow(subGrad[k], 2);
//     }

//     double step = pi*(node.ub - node.lb)/sumGradSquared;

//     for (int k=0; k<dat->edges.size(); k++) {
//       node.lagMult[k] = std::max(0.0, node.lagMult[k] + step*subGrad[k]);
//     }
//   }

//   // Getting upperbound using optimal lagrangian solution
//   schedule_orders(currentSol, dat);
//   if (currentSol.ub < node.ub) {
//     currentSol.copy_to(node.sol);
//     node.ub = node.sol.ub;

//     if (std::abs(node.ub - node.lb)/(1 + std::abs(node.lb)) <= 0.0001) {
//       node.time =  static_cast<double>((clock() - start)/CLOCKS_PER_SEC);
//       return true;
//     }
//   }

//   node.gap = (node.ub - node.lb)/node.ub;
//   return false;  
// }