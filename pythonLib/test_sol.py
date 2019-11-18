# -*- coding: utf-8 -*-
class Job:

    def __init__(self, p=0, w=0.0):
        self.p = p
        self.w = w
        self.prec = []

def check(f_name):
    file = open(f_name, 'r')

    instance = file.readline()[:-1]
    print(instance)

    while (instance):
        data = read_instance(instance)
        of = float(file.readline())
        orderJ1 = [int(i) for i in file.readline().split(',')]
        orderJ2 = [int(i) for i in file.readline().split(',')]
        real_of = test_sol(data, orderJ1, orderJ2)

        if (abs(of - real_of) > 1):
            print(instance + " " + f_name + " " + "OF não bate")
            print(of, real_of, of-real_of)
            print(order)
            print("\n")

        instance = file.readline()[:-1]
        print(instance)

def read_instance(inst_name):
    file = open('instancias/G1/' + inst_name, 'r')
    data = []

    n1, n2 = [int(i) for i in file.readline().split()]
    [data.append(Job(int(i), 0.0)) for i in file.readline().split()]

    for _ in range(n2):
        line = file.readline().split(' ')
        j = Job(0, float(line[0]))
        j.prec = [int(i) for i in line[1:]]
        data.append(j)

    return data

def test_sol(data, orderJ1, orderJ2):
    of = 0.0
    compTime = {}
    t = 0

    if (len(orderJ1) > len(set(orderJ1))) or (len(orderJ2) > len(set(orderJ2))):
        print(">>>>>>>> Falhou: Duplicada")
        return False

    if (set(orderJ1 + orderJ2) != set(range(len(data)))):
        print(">>>>>>>> Falhou: Falta elemento")
        return False


    for i in orderJ1:
        t += data[i].p
        compTime[i] = t

    start=0
    for j in orderJ2:
        for i in data[j].prec:
            if start < compTime[i]:
                start = compTime[i]

        of += data[j].w * start

    return of


if __name__ == "__main__":
    check('solutions/H1_naive.txt')
    check('solutions/H1.txt')
    check('solutions/H2.txt')
    check('solutions/H2M_50.txt')
