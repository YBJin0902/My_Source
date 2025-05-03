from numpy import genfromtxt
import numpy as np
import matplotlib.pyplot as plt
import numba
import pandas as pd
import numpy.ma as ma
import time
from time import sleep

def load_RSSI():
    output = np.empty((M_Candidate,46*98), dtype=np.float64)
    for i in range(M_Candidate):
        my_data = genfromtxt('./ap%s,5GHz.csv' %i, delimiter=',', skip_header=True)
        my_data = my_data.flatten()
        output[i] = my_data
    return output

@numba.jit(parallel=True, fastmath=True)
def normalize(data):
    data = (data - RSSI_MIN)/(RSSI_MAX - RSSI_MIN)
    return data

@numba.jit(parallel=True, fastmath=True)
def fitness_f(data):
    P_data = data.reshape(N_APS, -1)
    topK_min = np.argpartition(P_data, -top_K, axis=0)[-top_K:,:]#
    data_topK_min = P_data[topK_min, np.arange(P_data.shape[1])[None, :]]
    data_topK_not_nan =  np.all(~np.isnan(data_topK_min), axis=0) #shape (1,M)
    data_rssi_not_qualified = np.any(np.logical_and(data_topK_min <= RSSI_MIN, data_topK_not_nan) ,axis=0) # shape(1, M)
    P = (np.count_nonzero(data_rssi_not_qualified) / np.count_nonzero(data_topK_not_nan))
    data[data<=RSSI_MIN] = RSSI_MIN
    data = normalize(data)

    r_total = np.nansum(data)
    r_mean = r_total / (np.count_nonzero(~np.isnan(data)))
    v_total = np.nansum((data-r_mean)**2)
    r_variance = v_total / (np.count_nonzero(~np.isnan(data)))

    data_T = data.T
    mask_data = ma.masked_invalid(np.nan_to_num(data_T))
    mask_data_minus_mean = mask_data - np.mean(mask_data, axis=0)
    cov_matrix = np.cov(mask_data_minus_mean.T)
    eig_values, eig_vectors = np.linalg.eig(cov_matrix)
    P_eig_values = eig_values / eig_values.sum()
    entropy = (-P_eig_values*np.log2(P_eig_values)).sum()
    J = w1 * 1/(r_mean+1e-8) + w2 * (r_variance) - w3 * entropy+ np.exp(w4 * P)

    return J, r_mean, r_variance, P_eig_values, entropy, P



M_Candidate = 132 # Candidate size
N_APS = 20  # Number of AP   
CROSS_RATE = 0.8    # Crossover rate   
MUTATE_RATE = 0.02  # Mutation rate
POP_SIZE = 200  # Population size
N_GENERATIONS = 600   # Number of generation
RSSI_MAX = -30  # Maximum RSSI
RSSI_MIN = -80  # Minimum RSSI

w1 = 0.8    # Weight for r_mean
w2 = 0.2    # Weight for r_variance
w3 = 10     # Weight for S
w4 = 100    # Weight for P
top_K = 20 
cross_last_K = N_APS

class GA(object):
    def __init__(self, DNA_size, cross_rate, mutation_rate, pop_size, ):
        self.DNA_size = DNA_size
        self.cross_rate = cross_rate
        self.mutate_rate = mutation_rate
        self.pop_size = pop_size
        self.candidate = [i for i in range(M_Candidate)]
        self.pop = np.vstack([sorted(np.random.choice(self.candidate, N_APS, replace=False,)) for _ in range(pop_size)])
        self.RSSI_MAP = load_RSSI()

    def translateDNA(self, DNA):
        return DNA

    def get_fitness(self, DNA):
        
        fitness = np.empty((DNA.shape[0],), dtype=np.float64)
        for i, AP in enumerate(DNA):
            Configurations = self.RSSI_MAP[AP]
            fitness[i], r_mean, r_variance, P_eig_values, entropy, P = fitness_f(Configurations)
        print('get fitness shape', fitness.shape)
        return fitness

    def select(self, most_idx):
        idx = np.random.choice(most_idx, size=10, replace=True)
        return self.pop[idx]

    def crossover(self, parent, pop):
        if np.random.rand() < self.cross_rate:
            i_ = np.random.randint(0, cross_last_K, size=1)                        
            j_ = np.random.randint(0, cross_last_K, size=1)                       
            all_DNA = np.unique(np.array((parent[i_].flatten(), pop[j_].flatten())))    # mix parent DNA
            parent = np.array(sorted(np.random.choice(all_DNA,  N_APS, replace=False,)))    # choose crossover points
            return parent
        else:
            i_ = np.random.randint(0, cross_last_K, size=1)                      
            return parent[i_].flatten()

    def mutate(self, child):

        for point in range(self.DNA_size):
            if np.random.rand() < self.mutate_rate:
                main_list = np.setdiff1d(self.candidate,child)
                swap_point = np.random.choice(main_list,  1)[0]
                child[point] = swap_point
        return child

    def evolve(self, fitness):
        least_idx = fitness.argsort()[-cross_last_K:][::-1]
        most_idx =  fitness.argsort()[:cross_last_K][::-1]
        pop = self.pop[most_idx]
        pop_copy = pop.copy()
        for parent in least_idx:  # for every parent
            child = self.crossover(pop, pop_copy)
            child = self.mutate(child)
            self.pop[parent] = child

class TSP(object):
    def __init__(self, n_):
        self.RSSI_MAP = normalize(load_RSSI().reshape(M_Candidate, 46, 98))
        plt.ion()

    def plotting(self, best_idx, fitness):
        plt.cla()
        output = self.RSSI_MAP[best_idx]
        output = np.mean(self.RSSI_MAP, axis=0)
        plt.text(-0.05, -0.05, "Total distance=%.2f" % fitness, fontdict={'size': 20, 'color': 'red'})
        plt.imshow(output, interpolation=None,  cmap='Reds')
        plt.pause(0.01)


ga = GA(DNA_size=N_APS, cross_rate=CROSS_RATE, mutation_rate=MUTATE_RATE, pop_size=POP_SIZE)

env = TSP(N_APS)

start_time = time.time()
for generation in range(N_GENERATIONS):
    DNA = ga.translateDNA(ga.pop)
    fitness = ga.get_fitness(DNA)
    ga.evolve(fitness)
    best_idx = np.argmin(fitness)
    print('Gen:', generation, '| best fit: %f' % fitness[best_idx], '| worst fit: %f' % np.max(fitness), '| avg fit: %f' % np.mean(fitness))
    fitness_, r_mean, r_variance, P_eig_values, entropy, P = fitness_f(ga.RSSI_MAP[sorted(ga.pop[best_idx])])
    print("Configuration:", sorted(ga.pop[best_idx]), 'mean:', r_mean ,"variance:", r_variance, "P_eig_values", P_eig_values, "entropy:", entropy, "P:", P)

    env.plotting(sorted(ga.pop[best_idx]),fitness[best_idx])

end_time = time.time()


print('elapsed time', (end_time - start_time))