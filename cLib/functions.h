#pragma once

#include "structures.h"
#include <vector>

Solution smith_naive (Instance*);

Solution smith_naive_plus (Instance*);

Solution smith_dinamic (Instance* dat);

Solution priority_scheduling (Instance* dat);

Solution assemble_heuristic (Instance* dat);

// Solution jobs_scheduling_heuristic(Instance*, std::vector<int>&);

// Solution solve_subproblem(Node *node, Instance *dat);

// void schedule_orders(Solution&, Instance*);
