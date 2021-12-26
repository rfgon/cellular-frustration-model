#include "../include/utils.h"
#include "../include/cfmodel.h"
#include "../include/training.h"

using namespace cfm;

int main()
{
    // Read parameters from file
    std::map<std::string, int> params = parseParameters("../cellular-frustration-model/input/parameters.txt");

    // Number of presenters sets
    uint16_t const n_presenters_sets = params["presenters sets"];

    // Number of iterations analysing a sample
    uint16_t const sample_rounds = params["sample rounds"];

    // Load training set
    const std::vector<std::vector<float>> training_set = loadFloatMatrix("../cellular-frustration-model/input/training_set.csv");

    // Number of samples
    uint16_t n_samples = training_set.size();

    // Number of features
    uint16_t const n_features = training_set.at(0).size();

    // Number of presenters
    uint16_t const n_presenters = n_features * n_presenters_sets;

    // Number of detectors
    uint16_t const n_detectors = n_presenters;

    // Number of agents
    uint16_t const n_agents = n_presenters + n_detectors;

    // Initialize agents
    Agents agents = initAgents(n_agents);

    // Load untrained detectors' global lists
    std::vector<std::vector<uint16_t>> detectors_global_lists = loadUnsignedIntMatrix("../cellular-frustration-model/input/untrained_global_lists.csv");

    initDetectorsGlobalLists(agents, n_presenters, detectors_global_lists);

    // Load detectors' left/right critical values lists
    std::vector<std::vector<float>> left_criticals = loadFloatMatrix("../cellular-frustration-model/input/left_criticals.csv");

    std::vector<std::vector<float>> right_criticals = loadFloatMatrix("../cellular-frustration-model/input/right_criticals.csv");

    initDetectorsCriticalLists(agents, n_presenters, left_criticals, right_criticals);

    // Flag to execute the training portion of the program
    bool const train_flag = params["train"];

    if (train_flag) {
        // Load samples queue
        std::vector<uint16_t> samples_queue = loadUnsignedIntVector("../cellular-frustration-model/input/samples_queue.csv");

        // Number of iterations
        uint32_t const frustration_rounds = params["frustration rounds"];

        // File used to write all the agents' registered taus
        std::ofstream agents_taus_file("../cellular-frustration-model/output/untrained_taus.csv");

        // Interval of iterations between each training session
        uint16_t const training_interval = params["training interval"];

        // Dynamics with untrained detectors
        training(agents, n_presenters, frustration_rounds, sample_rounds, n_samples, samples_queue, n_features, training_set, training_interval, false);

        // Export agents' taus
        for (auto const& agent_map : agents.taus_map) {
            exportMap(agents_taus_file, agent_map);
        }
        agents_taus_file.close();

        // Reset some of the agents' data structures
        resetAgentsMatch(agents);
        resetAgentsTau(agents);
        resetAgentsTausMap(agents);

        // Number of iterations
        uint32_t const training_rounds = params["training rounds"];

        // Dynamics with detectors training
        training(agents, n_presenters, training_rounds, sample_rounds, n_samples, samples_queue, n_features, training_set, training_interval);

        // File used to write all the detectors' global lists
        std::ofstream detectors_global_lists_file("../cellular-frustration-model/input/trained_global_lists.csv");

        // Export detectors' global lists
        detectors_global_lists = {agents.global_list.begin() + n_presenters, agents.global_list.end()};
        for (auto const& global_list : detectors_global_lists) {
            exportVector(detectors_global_lists_file, global_list);
        }

        // Reset some of the agents' data structures
        resetAgentsMatch(agents);
        resetAgentsTau(agents);
        resetAgentsTausMap(agents);

        agents_taus_file.open("../cellular-frustration-model/output/trained_taus.csv");

        // Dynamics with trained detectors
        training(agents, n_presenters, frustration_rounds, sample_rounds, n_samples, samples_queue, n_features, training_set, training_interval, false);

        // Export agents' taus
        for (auto const& agent_map : agents.taus_map) {
            exportMap(agents_taus_file, agent_map);
        }
        agents_taus_file.close();

        // Reset some of the agents' data structures
        resetAgentsMatch(agents);
        resetAgentsTau(agents);
        resetAgentsTausMap(agents);

    }

    // Flag to execute the monitoring portion of the program
    bool const monitor_flag = params["monitor"];

    if (monitor_flag) {
        // Load trained detectors' global lists
        detectors_global_lists = loadUnsignedIntMatrix("../cellular-frustration-model/input/trained_global_lists.csv");

        initDetectorsGlobalLists(agents, n_presenters, detectors_global_lists);

        // Load test set
        const std::vector<std::vector<float>> test_set = loadFloatMatrix("../cellular-frustration-model/input/test_set.csv");

        // Load test set classes
        const std::vector<std::vector<short int>> test_set_classes_matrix = loadShortIntMatrix("../cellular-frustration-model/input/test_set_classes.csv");
        std::vector<short int> test_set_classes;
        for (auto const& row : test_set_classes_matrix) {
            for (auto const& val : row) {
                test_set_classes.push_back(val);
            }
        }
    }

    return 0;
}
