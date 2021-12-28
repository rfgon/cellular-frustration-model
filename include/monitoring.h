#ifndef MONITORING_H
#define MONITORING_H

#include "cfmodel.h"
#include <iterator> // iterator, next

namespace cfm
{

    // Compute a map's right to left cumulative sum
    template<class T1, class T2>
    std::map<T1, T2> computeMapCumulativeSum(const std::map<T1, T2>& map_data)
    {
        std::map<T1, T2> map_data_temp = map_data;
        for (auto it = std::next(map_data_temp.rbegin(), +1); it != map_data_temp.rend(); ++it) {
            auto tau_right = std::next(it, -1)->second;
            map_data_temp[it->first] += tau_right;
        }

        return map_data_temp;
    }

    // Compute activation tau based on the calibration samples
    uint16_t computeActivationTau(Agents& agents, uint16_t const& n_presenters, const std::vector<std::map<uint16_t, uint32_t>>& calibration_taus_map, uint16_t const& n_calibration_samples)
    {
        // Aggregate all the detectors' taus maps into one map
        std::map<uint16_t, float> aggregate_taus_map;
        const std::vector<uint16_t> detectors_ids = {agents.id.begin() + n_presenters, agents.id.end()};
        for (auto const& id : detectors_ids) {
            for (auto const& kv : calibration_taus_map.at(id)) {
                aggregate_taus_map[kv.first] += kv.second;
            }
        }

        // Cumulative sum of taus
        aggregate_taus_map = computeMapCumulativeSum(aggregate_taus_map);

        // Average of taus per sample per agent considered for the calibration
        for (auto& kv : aggregate_taus_map) {
            kv.second /= n_calibration_samples;
            kv.second /= detectors_ids.size();

            // Find the first activation tau that meets criteria
            if (kv.second < 1) {
                return kv.first;
            }
        }

        return aggregate_taus_map.rbegin()->first;
    }

    // Register the number of pairings for the activation tau after the monitoring of a sample
    void getNumberPairingsForActivationTau(Agents& agents, uint16_t const& n_presenters, std::vector<std::vector<uint32_t>>& number_pairings, uint16_t const& activation_tau)
    {
        // Loop through detectors
        const std::vector<uint16_t> detectors_ids = {agents.id.begin() + n_presenters, agents.id.end()};
        for (auto const& id : detectors_ids) {
            // Cumulative sum of taus
            agents.taus_map.at(id) = computeMapCumulativeSum(agents.taus_map.at(id));

            // Get the number of pairings for the activation tau, otherwise its zero
            auto it = agents.taus_map.at(id).lower_bound(activation_tau);
            if (it != agents.taus_map.at(id).end()) {
                number_pairings.at(id - n_presenters).push_back(it->second);
            } else {
                number_pairings.at(id - n_presenters).push_back(0);
            }
        }
    }

    // Compute activation threshold of each detector based on its number of pairings list
    void computeActivationThresholds(Agents& agents, uint16_t const& n_presenters, const std::vector<std::vector<uint32_t>>& number_pairings, uint16_t const& activation_threshold_percent, uint16_t const& n_normal_samples)
    {
        // Sort number of pairings from highest to lowest
        std::vector<std::vector<uint32_t>> number_pairings_sorted = number_pairings;
        for (auto& row : number_pairings_sorted) {
            std::sort(row.rbegin(), row.rend());
        }

        // Compute each detector's activation threshold
        const std::vector<uint16_t> detectors_ids = {agents.id.begin() + n_presenters, agents.id.end()};
        for (auto const& id : detectors_ids) {
            agents.activation_thresholds.at(id) = number_pairings_sorted.at(id - n_presenters).at((uint16_t)((n_normal_samples - 1) * (float)activation_threshold_percent / 100));
        }
    }

    // Compute the collective response of the detectors towards a test sample
    uint32_t computeCollectiveResponse(Agents const& agents, uint16_t const& n_presenters, uint16_t const& activation_tau)
    {
        // Collective response
        uint32_t response_sum = 0;

        // Loop through detectors
        const std::vector<uint16_t> detectors_ids = {agents.id.begin() + n_presenters, agents.id.end()};
        for (auto const& id : detectors_ids) {

            // Cumulative sum of taus
            std::map<uint16_t, uint32_t> agent_taus_map_temp = agents.taus_map.at(id);
            agent_taus_map_temp = computeMapCumulativeSum(agent_taus_map_temp);

            // Get the number of pairings for the activation tau, otherwise its zero
            auto it = agent_taus_map_temp.lower_bound(activation_tau);
            uint32_t number_pairings = 0;
            if (it != agent_taus_map_temp.end()) {
                number_pairings = it->second;
            }

            // Compute individual response and add it to collective response
            response_sum += (number_pairings - agents.activation_thresholds.at(id)) * (number_pairings > agents.activation_thresholds.at(id));
        }

        return response_sum;
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
