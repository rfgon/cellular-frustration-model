#ifndef TRAINING_H
#define TRAINING_H

#include "cfmodel.h"
#include <cassert>  // assert

namespace cfm
{

    // Decrease the current signal's rank in the detector's global list
    void decreaseSignalRank(Agents& agents, unsigned short int const& detector, unsigned short int const& signal, unsigned short int const& old_rank, unsigned short int const& new_rank)
    {
        // Update all the signals' ranks
        for (auto& rank : agents.global_list.at(detector)) {
            if (rank > old_rank && rank <= new_rank) {
                --rank;
            }
        }

        // Update the educated signal's rank
        agents.global_list.at(detector).at(signal) = new_rank;
    }

    // Educate detectors' global lists
    void education(std::mt19937& generator, Agents& agents, unsigned short int const& n_presenters, unsigned short int& threshold)
    {
        // Check if at least one detector was trained
        bool trained = false;

        // Loop through all detectors
        unsigned short int max_tau = 0;
        for (unsigned short int i = n_presenters; i < agents.id.size(); ++i) {
            // Find the highest tau among all detectors
            unsigned short int detector_tau = agents.tau.at(i);
            if (detector_tau > max_tau) {
                max_tau = agents.tau.at(i);
            }

            // Train detector if its tau is higher than threshold
            if (detector_tau > threshold) {
                trained = true;

                short int detector_partner = agents.match.at(i);
                assert(detector_partner > -1 && "Detector must be paired");
                assert(agents.match.at(detector_partner) == i && "Agents must be paired to same agent");
                assert(agents.tau.at(i) == agents.tau.at(detector_partner) && "Paired agents must have same tau");

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

    // Cellular frustration dynamics with detector training
    void training(unsigned short int const& seed, Agents& agents, unsigned short int const& n_presenters, unsigned int const& frustration_rounds, unsigned short int const& sample_rounds, unsigned short int const& n_samples, const std::vector<unsigned short int>& samples_queue, unsigned short int const& n_features, const std::vector<std::vector<float>>& data_set, unsigned short int const& training_interval)
    {
        // Initialize random number generator
        std::mt19937 generator(seed);

        // Sample counter used to loop samples
        unsigned int sample_counter = 0;

        // Initialize interactions queue (indices = priority; elements = interaction pairs)
        std::vector<unsigned short int> interactions_queue(n_presenters);
        std::iota(interactions_queue.begin(), interactions_queue.end(), 0);

        // Initialize interaction pairs (indices = presenters' ids; elements = detectors' ids)
        std::vector<unsigned short int> interaction_pairs(n_presenters);
        std::iota(interaction_pairs.begin(), interaction_pairs.end(), n_presenters);

        // Initialize education threshold
        unsigned short int threshold = training_interval;

        // Main loop
        for (unsigned int round = 0; round < frustration_rounds; ++round) {
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
            if (round % training_interval == 0 && round > 0) {
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
