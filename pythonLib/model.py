from amplpy import AMPL, DataFrame
import time
import numpy as np

class Model:
    def __init__(self, data, model_type, relax=False, time_limit=600):
        self.dat = data
        self.ampl = AMPL()        
        self.ampl.setOption('solver', 'cplex')
        self.ampl.setOption('cplex_options', 'timelimit={:d} return_mipgap=1 bestbound'.format(time_limit))
        self.relax = relax
        if relax:
            self.ampl.setOption('relax_integrality', 1)

        if model_type == "pv":
            self.ampl.read('ampl_models/pos_variables.mod')
        elif model_type == "ct":
            self.ampl.read('ampl_models/conc_time.mod')
        else:
            print("Invalid model_type")
            return

        self.bind_data()

    def bind_data(self):
        self.ampl.set['J1'] = ["A" + str(i) for i in self.dat.jobs]
        self.ampl.set['J2'] = ["B" + str(j) for j in self.dat.orders]
        edges = [('A' + str(i), 'B' + str(j)) for i,j in self.dat.edges]
        self.ampl.set['A'] = edges

        self.ampl.param['p'] = self.dat.p
        self.ampl.param['w'] = self.dat.w

    def solve(self):
        start = time.time()
        self.ampl.solve()

        self.elapsedTime = time.time() - start
        print(self.ampl.obj['COST'].result())
        # self.lb = self.ampl.obj['COST'].ub()
       # self.gap = self.ampl.getData('COST.relmipgap').toList()[0]

    def write_result(self, file):
        file.write(str(self.dat.g)+','+str(self.dat.n)+','+str(self.dat.m)+','+str(self.dat.i)+','+str(self.elapsedTime)+','+str(self.ub))

        if not self.relax:
            file.write(',')
            if self.lb:
                file.write(str(self.lb) + ',')
            else:
                file.write(',')

            if np.isfinite(self.gap):
                file.write(str(round(self.gap, 6)) + ',')
            else:
                file.write(',')

        file.write("\n")


    
    


