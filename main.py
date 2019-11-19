from pythonLib import Instance, Model, make_inst

if __name__ == "__main__":
    # ouput_file = open('results/MT.txt', 'w')
    # ouput_file.write("G,N,M,I,Time,UB,LB,Gap\n")

    dat = Instance('instancias/CM/01-80-48-00')
    model = Model(dat, model_type='ct', time_limit=180, relax=True)
    model.solve()
    print(model.lb)
    # model.write_result(ouput_file)
    # ouput_file.close()
    


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
