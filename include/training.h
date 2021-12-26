#ifndef TRAINING_H
#define TRAINING_H

#include "cfmodel.h"

namespace cfm
{

    // Decrease the current signal's rank in the detector's global list
    void decreaseSignalRank(Agents& agents, uint16_t const& detector, uint16_t const& signal, uint16_t const& old_rank, uint16_t const& new_rank)
    {
        // Update all the signals' ranks displaced by the educated signal
        for (auto& rank : agents.global_list.at(detector)) {
            if (rank > old_rank && rank <= new_rank) {
                --rank;
            }
        }

        // Update the educated signal's rank
        agents.global_list.at(detector).at(signal) = new_rank;
    }

    // Educate detectors' global lists
    void education(std::mt19937& generator, Agents& agents, uint16_t const& n_presenters, uint16_t& threshold)
    {
        // Check if at least one detector was trained
        bool trained = false;

        // Loop through all detectors
        uint16_t max_tau = 0;
        for (uint16_t i = n_presenters; i < agents.id.size(); ++i) {
            // Find the highest tau among all detectors
            uint16_t detector_tau = agents.tau.at(i);
            if (detector_tau > max_tau) {
                max_tau = agents.tau.at(i);
            }

            // Train detector if its tau is higher than threshold
            if (detector_tau > threshold) {
                trained = true;

                short int detector_partner = agents.match.at(i);

                decreaseSignalRank(agents, i, agents.local_list.at(i).at(detector_partner), getSignalRank(agents, n_presenters, i, detector_partner), agents.global_list.at(i).size() - 1);

                // Unpair detector from presenter with signal that caused a lasting pairing
                updateAgentMatch(agents, i, -1);
                updateAgentMatch(agents, detector_partner, -1);
            }
        }

        // Update threshold if no detector was trained
        if (!trained) {
            threshold = max_tau;
        }
    }

    // Cellular frustration dynamics with detector training by default
    void training(Agents& agents, uint16_t const& n_presenters, uint32_t const& frustration_rounds, uint16_t const& sample_rounds, uint16_t const& n_samples, const std::vector<uint16_t>& samples_queue, uint16_t const& n_features, const std::vector<std::vector<float>>& data_set, uint16_t const& training_interval, bool const& training_flag = true, uint16_t const& seed = 0)
    {
        // Initialize random number generator
        std::mt19937 generator(seed);

        // Initialize interactions queue (indices = priority; elements = interaction pairs)
        std::vector<uint16_t> interactions_queue(n_presenters);
        std::iota(interactions_queue.begin(), interactions_queue.end(), 0);

        // Initialize interaction pairs (indices = presenters' ids; elements = detectors' ids)
        std::vector<uint16_t> interaction_pairs(n_presenters);
        std::iota(interaction_pairs.begin(), interaction_pairs.end(), n_presenters);

        // Sample counter used to loop samples
        uint32_t sample_counter = 0;

        // Initialize education threshold
        uint16_t threshold = training_interval;

        // Main loop
        for (uint32_t round = 0; round < frustration_rounds; ++round) {
            // Loop through samples
            if (round % sample_rounds == 0) {
                changeSample(agents, n_presenters, n_samples, n_features, data_set.at(samples_queue.at(sample_counter++)), sample_counter);
            }

            // Loop through interactions between pairs of agents
            interactions(generator, agents, n_presenters, interactions_queue, interaction_pairs);

            // Randomly dissociate agents
            dissociation(generator, agents);

            // Update agents' metrics
            updateAgentsMetrics(agents);

            // Train eligible detectors
            if (training_flag && round % training_interval == 0 && round > 0) {
                education(generator, agents, n_presenters, threshold);
            }
        }

        // Register taus on last round
        for (auto const& id : agents.id) {
            ++agents.taus_map.at(id)[agents.tau.at(id)];
        }
    }

} // namespace cfm

#endif // TRAINING_H
