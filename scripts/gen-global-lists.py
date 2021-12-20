import numpy as np

# Load parameters
params = np.loadtxt("input/parameters.txt", dtype=str, delimiter=':')

# Random numbers seed
seed = int(params[params[:, 0] == "seed", 1])
rng = np.random.default_rng(seed)

# Load training set
training_set = np.loadtxt("input/training_set.csv", dtype=float, delimiter=',')

# Number of features
n_features = training_set.shape[1]

# Number of presenters sets
n_presenters_sets = int(params[params[:, 0] == "presenters sets", 1])

# Number of presenters/detectors
n_presenters = n_detectors = n_features * n_presenters_sets

# Build detectors' global lists
global_lists = np.arange(2*n_presenters, dtype=int)
global_lists = np.tile(global_lists, reps=(n_detectors, 1))

# Shuffle global lists
global_lists = rng.permuted(global_lists, axis=1)

# Export global lists
np.savetxt("input/untrained_global_lists.csv", global_lists, fmt='%i', delimiter=',')
