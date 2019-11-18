#include <dirent.h>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>
#include "cLib/structures.h"
#include "cLib/functions.h"

void get_names(const char *dir_name, std::vector<std::string> &inst_names) {
  DIR* dir = opendir(dir_name);
  struct dirent *dp;
  std::string str(dir_name);

  while ((dp = readdir(dir)) != NULL) {
      if (dp->d_name[0] != '.') {
          inst_names.push_back(str + '/' + dp->d_name);
      }
  }

  closedir(dir);
  std::sort(inst_names.begin(), inst_names.end());
}

int main() {
    std::vector<std::string> filePaths;
    get_names("instancias/CH", filePaths);

    Instance dat;

    std::ofstream file("results/SN+.txt");
    file << "G " << "N " << "M " << "I " << "OF" << std::endl;
    
    for (std::string name : filePaths) {
        dat.read_file(name.c_str());

        Solution sol = smith_naive_plus(&dat);
        file << dat.g << " " << dat.n << " " << dat.m << " " << dat.i << " " << std::setprecision(15) << sol.ub << std::endl;        
    }
    file.close();

    
    // ;

    // Solution sol1 = smith_naive(&dat);
    // std::cout << sol1.ub << std::endl;
    
    // Solution sol2 = smith_naive_plus(&dat);
    // std::cout << sol2.ub << std::endl;

    // Solution sol3 = smith_dinamic(&dat);
    // std::cout << sol3.ub << std::endl;
    // sol3.print_sol();

    // Solution sol4 = priority_scheduling(&dat);
    // sol4.print_sol();
    // for (int i : sol4.jobSeq) {
    //     std::cout << i << " ";
    // }
    // std::cout << std::endl;

    // Solution sol5 = assemble_heuristic(&dat);
    // sol5.print_sol();
}


    // for (int i : sol4.ordSeq) {
    //     std::cout << i << "(" << sol4.ordConc[i] << ") ";
    // }
    // std::cout << std::endl;