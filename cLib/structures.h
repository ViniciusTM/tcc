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
	
	std::vector<int> succ;
};

// Orders
struct Order {
	int id;
	double w;
	int sumPred;

	std::vector<int> pred;
};

// Edge
struct Edge {
	int i, j;

	Edge(int, int);
};

// Instance
struct Instance {
	int g, n, m, i;
	std::vector<Job> jobs;
	std::vector<Order> orders;
	std::vector<Edge> edges;

	void read_file(const char*);
	void clear();
};

// Solution 
struct Solution {
	double ub, lb;
    Instance *dat;

	std::vector<int> jobSeq;
	std::vector<int> ordSeq;

	std::vector<int> jobConc;
	std::vector<int> ordConc;

	Solution();
	Solution(Instance*);
	void link_data(Instance*);
	void clear();
	void copy_to(Solution&);
	void print_sol();
};

// // Node
// struct Node {
// 	Solution sol;
// 	double ub, lb, gap, time;
// 	std::vector<double> lagMult;

// 	std::vector<int> fixed;
// 	Node* parent;
// 	int depth;

// 	Node();
// 	Node(Node*, Instance*, int);
// 	void link_data(Instance*);
// 	std::vector<Node> make_children(Instance*);
// 	void write_results(std::ofstream&, Instance*);
// 	void write_log(std::ofstream&, Instance*);
	
// 	bool operator<(const Node&) const;
// };

// // Solver
// struct Solver {
// 	double ub, lb;
// 	Instance *dat;
// 	Node root;
// 	std::priority_queue<Node, std::vector<Node>> queue;

// 	Solver(Instance*);
// 	Solution solve();
// 	bool calc_bounds(Node&);
// };