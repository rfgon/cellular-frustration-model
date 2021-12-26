#ifndef MONITORING_H
#define MONITORING_H

#include "cfmodel.h"

namespace cfm
{

    // Cellular frustration dynamics with trained detectors that monitor test samples
    void monitoring(Agents& agents, uint16_t const& n_presenters, uint32_t const& frustration_rounds, uint16_t const& n_features, const std::vector<float>& sample, uint16_t const& seed = 0)
    {
        // Initialize random number generator
        std::mt19937 generator(seed);

        // Initialize interactions queue (indices = priority; elements = interaction pairs)
        std::vector<uint16_t> interactions_queue(n_presenters);
        std::iota(interactions_queue.begin(), interactions_queue.end(), 0);

        // Initialize interaction pairs (indices = presenters' ids; elements = detectors' ids)
        std::vector<uint16_t> interaction_pairs(n_presenters);
        std::iota(interaction_pairs.begin(), interaction_pairs.end(), n_presenters);

        // Main loop
        for (uint32_t round = 0; round < frustration_rounds; ++round) {
            changeSample(agents, n_presenters, n_features, sample);

            // Loop through interactions between pairs of agents
            interactions(generator, agents, n_presenters, interactions_queue, interaction_pairs);

            // Randomly dissociate agents
            dissociation(generator, agents);

            // Update agents' metrics
            updateAgentsMetrics(agents);
        }

        // Register taus on last round
        for (auto const& id : agents.id) {
            ++agents.taus_map.at(id)[agents.tau.at(id)];
        }
    }

} // namespace cfm

#endif // MONITORING_H
