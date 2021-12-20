import numpy as np

# Load critical lists
left_criticals = np.genfromtxt("input/left_criticals.csv", dtype=float, delimiter=',')
right_criticals = np.genfromtxt("input/right_criticals.csv", dtype=float, delimiter=',')

# Number of features
n_features = left_criticals.shape[1]

# Number of detectors
n_detectors = left_criticals.shape[0]

# Random numbers seed
seed = 0
rng = np.random.default_rng(seed)

# Shuffle critical lists by feature
for feature in np.arange(n_features):
    for detector in np.arange(n_detectors):
        rand_detector = rng.integers(0, n_detectors)
        if detector != rand_detector:
            # Get detectors' left and right critical values
            detector_left_val = left_criticals[detector, feature]
            detector_right_val = right_criticals[detector, feature]

            rand_detector_left_val = left_criticals[rand_detector, feature]
            rand_detector_right_val = right_criticals[rand_detector, feature]

            # Swap detectors' left and right critical values
            left_criticals[detector, feature] = rand_detector_left_val
            right_criticals[detector, feature] = rand_detector_right_val

            left_criticals[rand_detector, feature] = detector_left_val
            right_criticals[rand_detector, feature] = detector_right_val

# Export data sets
np.savetxt("input/left_criticals.csv", left_criticals, fmt='%.6f', delimiter=',')
np.savetxt("input/right_criticals.csv", right_criticals, fmt='%.6f', delimiter=',')
