#include "../include/utils.h"

using namespace cfm;

int main()
{
    // Read parameters from file
    std::map<std::string,int> params = parseParameters("../cellular-frustration-model/input/parameters.txt");

    // Seed
    int const seed = params["seed"];

    // Number of presenters sets
    int const n_presenters_sets = params["presenters sets"];

    // Number of rounds spent with a given sample
    int const sample_rounds = params["sample rounds"];

    // Load training set
    std::vector<std::vector<float>> training_set = loadFloatMatrix("../cellular-frustration-model/input/training_set.csv");

    // Number of samples
    int n_samples = training_set.size();

    // Number of features
    int const n_features = training_set.at(0).size();

    // Number of presenters
    int const n_presenters = n_features * n_presenters_sets;

    // Number of detectors
    int const n_detectors = n_presenters;

    // Number of agents
    int const n_agents = n_presenters + n_detectors;

    return 0;
}