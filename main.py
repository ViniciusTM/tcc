from pythonLib import Instance, Model, make_inst
import time
import os

if __name__ == "__main__":
    with open('results/BH.txt', 'w') as file:
        file.write("G N M I Time LB UB\n")

    paths = os.listdir("instancias/CM")
    paths.sort(key= lambda x: (int(x.split('-')[1]), int(x.split('-')[2]), int(x.split('-')[0]) ))

    for path in paths:
        path = os.path.join("instancias/CM/", path)

        with open('results/BH.txt', 'a') as file:
            dat = Instance(path)
            model = Model(dat, model_type='pv', time_limit=1200, relax=False)
            
            model.solve()

            file.write("{:d} {:d} {:d} {:d} {:f} {:f}\n".format(dat.g, dat.n, dat.m, dat.i, model.elapsedTime, model.lb, model.ub))    


# if __name__ == "__main__":
#     # N = [20, 40, 80, 100]
#     # P = [0.6, 1.0, 1.4]
#     # G = [1, 2]

#     N = [20, 40, 80, 100]
#     P = [0.6, 1.0, 1.4]
#     G = [1, 2]

#     path = 'instancias/CM/'
#     parameters = [(n,p,g) for n in N for p in P for g in G]

#     for param in parameters:
#         [make_inst(param, i, path) for i in range(3)]
