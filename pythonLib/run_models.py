from amplpy import AMPL, DataFrame
import numpy as np
import time
import os
import re

def read_data(filePath):
	data = {}
	file = open(filePath, 'r')

	line = file.readline().split()
	data['n'] = int(line[0])
	data['m'] = int(line[1])
	data['i'] = int(line[2])
	data['g'] = int(filePath[12])

	data['J1'] = list(range(data['n']))
	data['J2'] = list(range(data['n'], data['n']+data['m']))

	line = file.readline().split()
	data['p'] = [int(line[i]) for i in range(data['n'])]
	data['h'] = sum(data['p'])
	

	data['w'] = []
	data['A'] = []
	for j,line in enumerate(file.readlines()):
		line = line.split()
		data['w'].append(float(line[0]))
		data['A'] += [(int(i), j+data['n']) for i in line[1:]]

	file.close()

	return data

def bind_data(model, data, modelType):
	model.set['J1'] = data['J1']
	model.set['J2'] = data['J2']
	model.set['A'] = data['A']

	model.param['p'] = data['p']
	model.param['w'] = data['w']

	if modelType == 'ti':
		model.param['h'] = data['h']

def check_solution(model, data, modelType):
	x = model.var['x'].getValues().toList()
	x = [(int(i), int(j)) for i,j,v in x if v ==1]

	obj = 0
	jobsConclusionTime = {}

	if modelType =='ti':
		y = [v for v in model.var['y'].getValues().toList() if v[2] == 1]
		c = [(t,v) for j,t,v in y]
		x.sort(key=lambda v: v[1])
		
		release = 0
		t = 0
		for k,t in x:
			if t < release:
				print(' Invalid Solution '.rjust(45, '='))
				print('job {:d} started at t={:d} but release date was {:d}'.format(int(k),int(t),releaseDate))
				return

			jobsConclusionTime[k] = t + data['p'][k]
	else:
		c = [(int(k), v) for k,v in model.var['c'].getValues().toList()]
		c = [(k,v) for k,v in c if k >= data['n']]
		for j in data['J1']:
			prec = [i for i in data['J1'] if (i,j) in data['A']]
			jobsConclusionTime[j] = sum([data['p'][i] for i in prec]) + data['p'][j]

	for j, v in c:
		for i, r in [(i, jobsConclusionTime[i]) for i in data['J1'] if (i,j) in data['A']]:
			if v < r:
				print(' Invalid Solution '.rjust(45, 'X'))
				print('job {:d} started at time {:.1f} before its predecessor {:d} ends at {:.f}'.format(j, v, i, r))
				return

		obj += v*data['w'][j - data['n']]
		print(j,v)

	modelObj = model.getObjective('COST').value()
	if abs(modelObj - obj) > 0.001:
		print(' Invalid Solution '.rjust(45, '='))
		print('Model COST ({:f}) does not match with from calculated COST ({:f})\n'.format(modelObj, obj))
		return

	print(' OK '.rjust(45, '='))

def solve_model(model, data, file, modelType, relax=False):
	model.reset()
	
	if modelType == 'ti':
		model.read('ampl_models/time_index.mod')
	elif modelType == 'pv':
		model.read('ampl_models/pos_variables.mod')
	else:
		print('\nERROR: Invalid modelYype {}\n'.format(modelType))

	bind_data(model, data, modelType)

	start = time.time()
	model.solve()
	elapsedTime = time.time() - start

	obj = model.obj['COST'].value()
	file.write(str(data['n']) + ',' + str(data['m']) + ',' + str(data['i']) + ',' + str(elapsedTime) + ',')

	if relax:
		file.write(str(obj) + '\n')

	else:
		msg = model.obj['COST'].message()
		msg = re.findall("\d+ branch-and-bound nodes", msg)[0]

		UB = model.obj['COST'].value()
		if UB:
			file.write(str(UB) + ',')
			check_solution(model, data, modelType)
		else:
			file.write(',')

		LB = model.getData('COST.bestbound').toList()[0]
		if LB:
			file.write(str(LB) + ',')
		else:
			file.write(',')

		gap = model.getData('COST.relmipgap').toList()[0]
		if np.isfinite(gap):
			file.write(str(round(gap, 6)) + ',')
		else:
			file.write(',')
	
		nNodes = int(re.findall("\d+", msg)[0])
		if nNodes: 
			file.write(str(nNodes) + '\n')
		else:
			file.write(str(nNodes + 1) + '\n')


if __name__ == '__main__':
	instPaths = ['instancias/G1/' + name for name in os.listdir('instancias/G1/')]

	outPath = 'output/MIP_PV_G1.txt'
	pvmipFile = open(outPath, 'w')
	pvmipFile.write('N,M,I,Time,UB,LB,Gap,B&B Nodes\n')

	outPath = 'output/LP_PV_G1.txt'
	pvlpFile = open(outPath, 'w')
	pvlpFile.write('N,M,I,Time,LB\n')

	model = AMPL()
	model.setOption('solver', 'cplex')

	for dataPath in ['instancias/G1/01-20-12-00']:
		print('Solving {:s}'.format(dataPath[-14:]).center(55, '='))
		data = read_data(dataPath)

		# Solving Linear Relaxation
		model.setOption('relax_integrality', 1)
		model.setOption('cplex_options', 'timelimit=1e75')

		print('Positon Variables Linear Relaxation'.format(dataPath[-14:]).rjust(45, '='))
		solve_model(model, data, pvlpFile, modelType='pv', relax=True)


		## Solving MIP models
		model.setOption('relax_integrality', 0)
		model.setOption('cplex_options', 'timelimit=600 return_mipgap=1 bestbound')

		print('Position Variables MIP'.format(dataPath[-14:]).rjust(45, '='))
		solve_model(model, data, pvmipFile, modelType='pv', relax=False)

		print('\n')

	pvmipFile.close()
	pvlpFile.close()

	del model