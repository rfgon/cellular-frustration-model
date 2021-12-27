#ifndef MONITORING_H
#define MONITORING_H

#include "cfmodel.h"
#include <iterator> // iterator, next

namespace cfm
{

    // Compute activation tau based on the calibration samples
    uint16_t computeActivationTau(Agents& agents, uint16_t const& n_presenters, const std::vector<std::map<uint16_t, uint32_t>>& calibration_taus_map, uint16_t n_calibration_samples)
    {
        // Aggregate all the detectors' taus maps into one map
        std::map<uint16_t, float> aggregate_taus_map;
        for (auto const& id : agents.id) {
            if (id >= n_presenters) {
                for (auto const& kv : calibration_taus_map.at(id)) {
                    aggregate_taus_map[kv.first] += kv.second;
                }
            }
        }

        // Number of agents considered for the calibration
        uint16_t n_calibration_agents = agents.id.size() - n_presenters;

        // Cumulative sum of taus
        for (auto it = std::next(aggregate_taus_map.rbegin(), +1); it != aggregate_taus_map.rend(); ++it) {
            auto tau_right = std::next(it, -1)->second;
            aggregate_taus_map[it->first] += tau_right;
        }

        // Average of taus per sample per agent considered for the calibration
        for (auto& kv : aggregate_taus_map) {
            kv.second /= (float)n_calibration_samples / n_calibration_agents;

            // Find the first activation tau that meets criteria
            if (kv.second < 1) {
                return kv.first;
            }
        }

        return aggregate_taus_map.rbegin()->first;
    }

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
