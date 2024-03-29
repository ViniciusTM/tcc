#include <dirent.h>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <chrono> 
#include "pottsLib/structures.h"
// #include "PottsLib/functions.h"

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

    // // ------------------------ Testes Lagrangiano ------------------------//
    // std::vector<std::string> filePaths;
    // get_names("instancias/CM", filePaths);

    // std::ofstream file("results/LR.txt");
    // file << "G " << "N " << "M " << "I " << "Time " << "LB" << std::endl;

    // Instance dat;    

    // for (std::string name : filePaths) {
    //     dat.read_file(name.c_str());
    //     file << dat.g << " " << dat.n << " " << dat.m << " " << dat.i << " ";       

        // auto start = std::chrono::high_resolution_clock::now();
        // double lb = lagrangean_relax(&dat);
        // auto finish = std::chrono::high_resolution_clock::now();
        // std::chrono::duration<double> elapsed = finish - start;

    //     file <<  std::setprecision(15) << elapsed.count() << " " << lb << std::endl;               
    // }
    // file.close();


    // ------------------------ Testes heurísticas ------------------------//
    std::vector<std::string> filePaths {"instancias/CM/01-20-20-01"};
    //get_names("instancias/CM", filePaths);

    Instance dat;

    // std::ofstream file("results/UB.txt");
    // file << "G " << "N " << "M " << "I " << "UB" << std::endl;
    
    for (std::string name : filePaths) {
        dat.read_file(name.c_str());


        // Solution sol = assemble_heuristic(&dat);
        // file << dat.g << " " << dat.n << " " << dat.m << " " << dat.i << " " << std::setprecision(15) << sol.ub << std::endl;        
    }
    // file.close();
}

