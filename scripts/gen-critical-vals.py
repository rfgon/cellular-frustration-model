import numpy as np

# Load parameters
params = np.loadtxt("input/parameters.txt", dtype=str, delimiter=':')

# Random numbers seed
seed = int(params[params[:,0]=="seed",1])
rng = np.random.default_rng(seed)

# nu values
max_nu = float(params[params[:,0]=="max nu",1])

# Load training set
training_set = np.loadtxt("input/training_set.csv", dtype=float, delimiter=',')

# Number of samples
n_samples = training_set.shape[0]

# Number of features
n_features = training_set.shape[1]

# Number of presenters sets
n_presenters_sets = int(params[params[:,0]=="presenters sets",1])

# Number of detectors
n_detectors = n_features * n_presenters_sets

# Load training set cluster labels
labels = np.loadtxt("input/labels.csv", dtype=int, delimiter=',')

# Unique clusters and number of samples in each one
clusters,clusters_n_samples = np.unique(labels, return_counts=True)

# Number of clusters
n_clusters = len(clusters)

# Empty clusters
clusters_samples = []

# Assign samples to clusters
for cluster,cluster_index in zip(clusters, range(n_clusters)):
    clusters_samples.append(np.empty((clusters_n_samples[cluster_index]), dtype=int))
    sample_count = 0
    for label,sample_index in zip(labels, range(n_samples)):
        if (label == cluster):
            clusters_samples[cluster_index][sample_count] = sample_index
            sample_count += 1

# Build samples queue
samples_queue = np.ravel(clusters_samples).reshape((1,n_samples))

# Export samples queue
np.savetxt("input/samples_queue.csv", samples_queue, fmt='%i', delimiter=',')

# Number of detectors in each cluster
detectors_in_cluster = np.array([np.ceil(n_detectors / n_clusters)]*n_clusters, dtype=int)
total_detectors_in_cluster = np.sum(detectors_in_cluster)
excess_detectors_in_cluster = total_detectors_in_cluster - n_detectors
detectors_in_cluster[-1] -= excess_detectors_in_cluster

# Clusters without assigned detectors
clusters_detectors = []

# Assign detectors to clusters
first_detector = 0
for cluster_index in range(n_clusters):
    last_detector = first_detector + detectors_in_cluster[cluster_index]
    clusters_detectors.append(np.arange(first_detector, last_detector, dtype=int))
    first_detector = last_detector

# Detectors' critical values lists
left_criticals = np.empty((n_detectors, n_features))
right_criticals = np.empty((n_detectors, n_features))

# Cycle through clusters
for cluster_index in range(n_clusters):
    # Build each detector's critical values lists
    for detector in clusters_detectors[cluster_index]:
        # Generate nu value
        nu = rng.uniform(low=0, high=max_nu)
        nu_half = nu / 2

        # Number of samples to the left and right of the normal domain
        n_samples_out_left = int((clusters_n_samples[cluster_index] - 1) * nu_half)
        n_samples_out_right = clusters_n_samples[cluster_index] - 1 - n_samples_out_left

        # Get critical values for each feature
        for feature in range(n_features):
            left_criticals[detector][feature] = np.partition(training_set[clusters_samples[cluster_index]][:,feature], n_samples_out_left)[n_samples_out_left]

            right_criticals[detector][feature] = np.partition(training_set[clusters_samples[cluster_index]][:,feature], n_samples_out_right)[n_samples_out_right]

# Export critical values lists
np.savetxt("input/left_criticals.csv", left_criticals, fmt='%.6f', delimiter=',')
np.savetxt("input/right_criticals.csv", right_criticals, fmt='%.6f', delimiter=',')