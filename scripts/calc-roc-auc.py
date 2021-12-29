import numpy as np
from sklearn.metrics import auc


def computeFprTpr(normal_responses, abnormal_responses):
    """Compute FPR and TPR values from responses"""

    # Thresholds
    thresholds = np.concatenate(([-1], normal_responses))

    # FPR for each normal response threshold
    fpr = []

    # TPR for each normal response threshold
    tpr = []

    # Cycle through every normal response threshold
    for threshold in thresholds:
        # Number of false positives
        fp = 0

        # Number of true positives
        tp = 0

        # Number of false negatives
        fn = 0

        # Number of true negatives
        tn = 0

        # Calculate false positives and true negatives
        for normal_response in normal_responses:
            if normal_response > threshold:
                fp += 1
            else:
                tn += 1

        # Calculate true positives and false negatives
        for abnormal_response in abnormal_responses:
            if abnormal_response > threshold:
                tp += 1
            else:
                fn += 1

        # Calculate FPR and TPR for this threshold
        fpr.append(fp / (fp + tn))
        tpr.append(tp / (tp + fn))

    return np.flip(fpr, axis=0), np.flip(tpr, axis=0)


# Load responses
responses = np.loadtxt("output/responses.csv", dtype=int, delimiter=',')

# Load classes of the samples corresponding to each response
responses_samples_classes = np.loadtxt("input/test_set_classes.csv", dtype=int, delimiter=',')

# Responses towards normal samples
normal_responses = responses[responses_samples_classes == -1]
normal_responses = np.sort(normal_responses)

# Responses towards abnormal samples
abnormal_responses = responses[responses_samples_classes == 1]
abnormal_responses = np.sort(abnormal_responses)

# Compute FPR and TPR from responses
fpr_vals, tpr_vals = computeFprTpr(normal_responses, abnormal_responses)

# Compute interpolated TPR values for a range of FPR values
fpr_interp = np.linspace(0, 1, 101)
tpr_interp = np.interp(fpr_interp, fpr_vals, tpr_vals)

# Get AUC for interpolated values
auc_val = auc(fpr_interp, tpr_interp)

# Export ROC curve
roc_vals = np.column_stack((fpr_interp * 100, tpr_interp * 100))
np.savetxt("output/roc_curve.csv", roc_vals, fmt=['%d', '%.2f'], delimiter=',')

# Export auc
np.savetxt("output/auc.csv", [auc_val * 100], fmt='%.2f', delimiter=',')
