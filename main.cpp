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

    // ------------------------ Testes Lagrangiano ------------------------//
    std::vector<std::string> filePaths = {"instancias/CM/01-80-48-00"};
    // get_names("instancias/CM", filePaths);

    Instance dat;

    for (std::string name : filePaths) {
        dat.read_file(name.c_str());

        double lb = lagrangean_relax(&dat);
        std::cout << lb << std::endl;          
    }


    //------------------------ Testes heurísticas ------------------------//
    // std::vector<std::string> filePaths;
    // get_names("instancias/CH", filePaths);

    // Instance dat;

    // std::ofstream file("results/SN+.txt");
    // file << "G " << "N " << "M " << "I " << "OF" << std::endl;
    
    // for (std::string name : filePaths) {
    //     dat.read_file(name.c_str());

    //     Solution sol = smith_naive_plus(&dat);
    //     file << dat.g << " " << dat.n << " " << dat.m << " " << dat.i << " " << std::setprecision(15) << sol.ub << std::endl;        
    // }
    // file.close();
}

