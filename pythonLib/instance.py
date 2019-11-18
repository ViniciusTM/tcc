import numpy as np

def make_inst(param, i, path):
    n = param[0]
    m = int(n*param[1])
    g = param[2]

    if (g==1):
        pn = np.random.randint(1, 11, n)
    else:
        pn = np.random.randint(10,101, n)

    card = np.random.randint(1, n-1, m)
    alpha = np.random.uniform(0.001, 1, m)
    prec = [np.random.choice(range(n), c, False) for c in card]

    f = open(path + '{:02d}-{:02d}-{:02d}-{:02d}'.format(g, n, m, i), 'w')
    f.write('{:d} {:d} {:d} {:d}\n'.format(g, n, m, i))
    f.write(' '.join([str(i) for i in pn]))
    f.write('\n')
    for i in range(m):
        f.write('{:.2f} '.format(alpha[i]))
        f.write(' '.join([str(j) for j in prec[i]]))
        f.write('\n')

    f.close()

class Instance:
    def __init__(self, file_path):
    
        file = open(file_path, 'r')

        line = file.readline().split()
        self.g = int(line[0])
        self.n = int(line[1])
        self.m = int(line[2])
        self.i = int(line[3])

        self.jobs = list(range(self.n))
        self.orders = list(range(self.m))

        line = file.readline().split()
        self.p = [int(line[i]) for i in range(self.n)]
        self.horizon = sum(self.p)

        self.w = []
        self.prec_matrix = np.zeros((self.n, self.m), dtype=bool)
        self.edges = []

        for j,line in enumerate(file.readlines()):
            line = line.split()
            self.w.append(float(line[0]))

            for i in line[1:]:
                i = int(i)
                self.prec_matrix[i][j] = True
                self.edges.append((i,j))
                
        file.close()

    def check_solution(self, sol):
        pass