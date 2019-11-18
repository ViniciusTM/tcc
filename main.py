from pythonLib import Instance, Model, make_inst

# if __name__ == "__main__":
#     dat = Instance('instancias/CH/01-20-28-00')
#     model = Model(dat, model_type='ct')
#     model.solve()


if __name__ == "__main__":
    # N = [20, 40, 80, 100]
    # P = [0.6, 1.0, 1.4]
    # G = [1, 2]

    N = [20, 40, 80]
    P = [0.6, 1.0, 1.4]
    G = [1, 2]

    path = 'instancias/CM/'
    parameters = [(n,p,g) for n in N for p in P for g in G]

    for param in parameters:
        [make_inst(param, i, path) for i in range(1)]
