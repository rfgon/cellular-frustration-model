#include "../include/utils.h"
#include "../include/cfmodel.h"

using namespace cfm;

int main()
{
    // Read parameters from file
    std::map<std::string, int> params = parseParameters("../cellular-frustration-model/input/parameters.txt");

    // Seed
    unsigned short int const seed = params["seed"];

    // Number of presenters sets
    unsigned short int const n_presenters_sets = params["presenters sets"];

    // Number of iterations analysing a sample
    unsigned short int const sample_rounds = params["sample rounds"];

    // Load training set
    const std::vector<std::vector<float>> training_set = loadFloatMatrix("../cellular-frustration-model/input/training_set.csv");

    // Number of samples
    unsigned short int n_samples = training_set.size();

    // Number of features
    unsigned short int const n_features = training_set.at(0).size();

    // Number of presenters
    unsigned short int const n_presenters = n_features * n_presenters_sets;

    // Number of detectors
    unsigned short int const n_detectors = n_presenters;

    // Number of agents
    unsigned short int const n_agents = n_presenters + n_detectors;

    // Initialize agents
    Agents agents = initAgents(n_agents);

    // Load detectors' global lists
    const std::vector<std::vector<unsigned short int>> global_lists = loadUnsignedIntMatrix("../cellular-frustration-model/input/untrained_global_lists.csv");

    initDetectorsGlobalLists(agents, n_presenters, global_lists);

    // Load detectors' left/right critical values lists
    std::vector<std::vector<float>> left_criticals = loadFloatMatrix("../cellular-frustration-model/input/left_criticals.csv");

    std::vector<std::vector<float>> right_criticals = loadFloatMatrix("../cellular-frustration-model/input/right_criticals.csv");

    initDetectorsCriticalLists(agents, n_presenters, left_criticals, right_criticals);

    // Load samples queue
    std::vector<unsigned short int> samples_queue = loadUnsignedIntVector("../cellular-frustration-model/input/samples_queue.csv");

    // Number of iterations
    unsigned int const frustration_rounds = params["frustration rounds"];

    // File used to write all the agents' registered taus
    std::ofstream agents_taus_file("../cellular-frustration-model/output/untrained_taus.csv");

    // Dynamics with untrained detectors
    cellularFrustration(seed, agents, n_presenters, frustration_rounds, sample_rounds, n_samples, samples_queue, n_features, training_set);

    // Export agents' taus
    for (auto const& agent_map : agents.taus_map) {
        exportMap(agents_taus_file, agent_map);
    }

    // Reset some of the agents' data structures
    resetAgentsMatch(agents);
    resetAgentsTau(agents);
    resetAgentsTausMap(agents);

    return 0;
}
