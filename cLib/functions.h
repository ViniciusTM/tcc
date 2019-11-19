#pragma once

#include "structures.h"
#include <vector>

Solution smith_naive (Instance*);

Solution smith_naive_plus (Instance*);

Solution smith_dinamic (Instance*);

Solution priority_scheduling (Instance*);

Solution assemble_heuristic (Instance*);

// Solution jobs_scheduling_heuristic(Instance*, std::vector<int>&);

double lagrangean_relax(Instance*);


// void schedule_orders(Solution&, Instance*);
