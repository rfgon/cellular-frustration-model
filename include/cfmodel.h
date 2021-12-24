#ifndef CFMODEL_H
#define CFMODEL_H

#include "utils.h"
#include <random>   // default_random_engine

namespace cfm
{

    // Agents' properties
    struct Agents
    {
        std::vector<unsigned short int> id;

        std::vector<unsigned short int> subtype;

        // Partner agent's id
        std::vector<short int> match;

        // Signal shown
        std::vector<float> signal;

        // Current matching lifetime
        std::vector<unsigned int> tau;

        // All registered matching lifetimes
        std::vector<std::map<unsigned int, unsigned int>> taus_map;

        // Global preference list
        std::vector<std::vector<unsigned short int>> global_list;

        // Local preference list
        std::vector<std::vector<unsigned short int>> local_list;

        // Features left critical values
        std::vector<std::vector<float>> left_criticals;

        // Features right critical values
        std::vector<std::vector<float>> right_criticals;

    };

    // Unmatch all agents
    void resetAgentsMatch(Agents& agents)
    {
        for (auto& match : agents.match) {
            match = -1;
        }
    }

    // Initialize agents' properties
    Agents initAgents(unsigned short int const& n_agents)
    {
        Agents agents;

        agents.id.resize(n_agents);
        agents.subtype.resize(n_agents);
        agents.match.resize(n_agents);
        agents.signal.resize(n_agents);
        agents.tau.resize(n_agents);
        agents.taus_map.resize(n_agents);
        agents.global_list.resize(n_agents);
        agents.local_list.resize(n_agents);
        agents.left_criticals.resize(n_agents);
        agents.right_criticals.resize(n_agents);

        for (unsigned short int i = 0; i < n_agents; ++i) {
            agents.id.at(i) = i;

            if (agents.id.at(i) < n_agents / 2) {
                // Different subtypes for each half of presenters
                if (agents.id.at(i) < n_agents / 4) {
                    agents.subtype.at(i) = 0;
                    agents.global_list.at(i) = {agents.subtype.at(i), 1};
                } else {
                    agents.subtype.at(i) = 1;
                    agents.global_list.at(i) = {agents.subtype.at(i), 0};
                }
            } else {
                // Different subtypes for each half of detectors
                if (agents.id.at(i) < n_agents * 3/4) {
                    agents.subtype.at(i) = 0;
                    agents.signal.at(i) = agents.subtype.at(i);
                } else {
                    agents.subtype.at(i) = 1;
                    agents.signal.at(i) = agents.subtype.at(i);
                }
            }

            // Unpaired agents
            resetAgentsMatch(agents);
        }

        return agents;
    }

    // Initialize detectors' global lists
    void initDetectorsGlobalLists(Agents& agents, unsigned short int const& n_presenters, const std::vector<std::vector<unsigned short int>>& global_lists)
    {
        unsigned short int i = 0;
        for (auto& row : global_lists) {
            agents.global_list.at(n_presenters + i++) = row;
        }
    }

    // Initialize detectors' critical values lists
    void initDetectorsCriticalLists(Agents& agents, unsigned short int const& n_presenters, const std::vector<std::vector<float>>& left_criticals, const std::vector<std::vector<float>>& right_criticals)
    {
        unsigned short int i = 0;
        for (auto& row : left_criticals) {
            agents.left_criticals.at(n_presenters + i++) = row;
        }
        i = 0;
        for (auto& row : right_criticals) {
            agents.right_criticals.at(n_presenters + i++) = row;
        }
    }

    // Map sample features to presenters' signals
    void mapSampleToPresentersSignals(Agents& agents, unsigned short int const& n_presenters, unsigned short int const& n_features, const std::vector<float>& sample)
    {
        unsigned short int feature = 0;
        for (unsigned short int i = 0; i < n_presenters; ++i) {
            // Reset feature counter at the end of every presenter set
            if (i % n_features == 0) {
                feature = 0;
            }
            agents.signal.at(i) = sample.at(feature++);
        }
    }

    // Map presenters' signals to detectors' local lists
    void mapSignalsToDetectorsLocalLists(Agents& agents, unsigned short int const& n_presenters, unsigned short int const& n_features)
    {
        // Loop through detectors
        for (unsigned short int i = n_presenters; i < agents.id.size(); ++i) {
            agents.local_list.at(i).resize(n_presenters);

            // Loop through presenters
            unsigned short int feature = 0;
            for (unsigned short int j = 0; j < n_presenters; ++j) {
                // Reset feature counter at the end of every presenter set
                if (feature == n_features) {
                    feature = 0;
                }

                // Get signal shown by each presenter
                float signal = agents.signal.at(j);

                // Get detector's critical values
                float left_critical = agents.left_criticals.at(i).at(feature);
                float right_critical = agents.right_criticals.at(i).at(feature++);

                // Signal out
                if (signal <= left_critical || signal >= right_critical) {
                    agents.local_list.at(i).at(j) = 2*j + 1;
                }
                // Signal in
                else {
                    agents.local_list.at(i).at(j) = 2*j + 0;
                }
            }
        }
    }

    // Change sample and map its features to signals
    void changeSample(Agents& agents, unsigned short int const& n_presenters, unsigned short int const& n_samples, unsigned short int const& n_features, const std::vector<float>& sample, unsigned int& sample_counter)
    {
        // Change presenters signals
        mapSampleToPresentersSignals(agents, n_presenters, n_features, sample);

        // Change detectors local lists
        mapSignalsToDetectorsLocalLists(agents, n_presenters, n_features);

        // Reset sample counter
        if (sample_counter == n_samples) {
            sample_counter = 0;
        }
    }

    // Update agent pairs and reset their tau counters
    void updateAgentPairs(Agents& agents, unsigned short int const& presenter, short int const& presenter_partner, unsigned short int const& detector, short int const& detector_partner)
    {
        // Pair agents and register/reset taus
        agents.match.at(presenter) = detector;
        agents.match.at(detector) = presenter;
        ++agents.taus_map.at(presenter)[agents.tau.at(presenter)];
        ++agents.taus_map.at(detector)[agents.tau.at(detector)];
        agents.tau.at(presenter) = 0;
        agents.tau.at(detector) = 0;

        // Unpair old partner agents and register/reset taus
        if (presenter_partner > -1) {
            agents.match.at(presenter_partner) = -1;
            ++agents.taus_map.at(presenter_partner)[agents.tau.at(presenter_partner)];
            agents.tau.at(presenter_partner) = 0;
        }

        if (detector_partner > -1) {
            agents.match.at(detector_partner) = -1;
            ++agents.taus_map.at(detector_partner)[agents.tau.at(detector_partner)];
            agents.tau.at(detector_partner) = 0;
        }
    }

    // Get the rank of an agent's signal in another agent's global list
    unsigned short int getSignalRank(Agents& agents, unsigned short int const& n_presenters, short int const& agent, short int const& agent_showing_signal)
    {
        // Presenters
        if (agent < n_presenters) {
            return agents.global_list.at(agent).at(agents.signal.at(agent_showing_signal));
        }
        // Detectors
        else {
            return agents.global_list.at(agent).at(agents.local_list.at(agent).at(agent_showing_signal));
        }
    }

    // Return true if a detector sees a presenter's signal as normal and false otherwise
    bool getSignalNormality(Agents& agents, unsigned short int const& detector, unsigned short int const& presenter)
    {
        if (agents.local_list.at(detector).at(presenter) % 2 == 0) {
            return true;
        }
        return false;
    }

    // Decision rules for pairing agents
    void decisionRules(Agents& agents, unsigned short int const& n_presenters, unsigned short int const& presenter, unsigned short int const& detector)
    {
        // Presenter's partner
        short int presenter_partner = agents.match.at(presenter);

        // Detector's partner
        short int detector_partner = agents.match.at(detector);

        if (detector_partner == -1) {
            if (presenter_partner == -1) { // Rule 1
                updateAgentPairs(agents, presenter, presenter_partner, detector, detector_partner);
            } else {
                // Check presenter's preference
                if (getSignalRank(agents, n_presenters, presenter, detector) < getSignalRank(agents, n_presenters, presenter, presenter_partner)) { // Rule 3
                    updateAgentPairs(agents, presenter, presenter_partner, detector, detector_partner);
                } else if (getSignalRank(agents, n_presenters, presenter, detector) == getSignalRank(agents, n_presenters, presenter, presenter_partner)) {
                    // Check how detectors see presenter's signal (as normal or abnormal)
                    if (getSignalNormality(agents, detector, presenter) && !getSignalNormality(agents, presenter_partner, presenter)) { // Rule 4
                        updateAgentPairs(agents, presenter, presenter_partner, detector, detector_partner);
                    }
                }
            }
        } else {
            if (presenter_partner == -1) {
                // Check detector's preference
                if (getSignalRank(agents, n_presenters, detector, presenter) < getSignalRank(agents, n_presenters, detector, detector_partner)) { // Rule 2
                    updateAgentPairs(agents, presenter, presenter_partner, detector, detector_partner);
                }
            } else {
                // Check detector's preference
                if (getSignalRank(agents, n_presenters, detector, presenter) < getSignalRank(agents, n_presenters, detector, detector_partner)) {
                    // Check presenter's preference
                    if (getSignalRank(agents, n_presenters, presenter, detector) < getSignalRank(agents, n_presenters, presenter, presenter_partner)) { // Rule 5
                        updateAgentPairs(agents, presenter, presenter_partner, detector, detector_partner);
                    } else if (getSignalRank(agents, n_presenters, presenter, detector) == getSignalRank(agents, n_presenters, presenter, presenter_partner)) {
                        // Check how detectors see presenters' signals (as normal or abnormal)
                        if (getSignalNormality(agents, detector, presenter) && !getSignalNormality(agents, detector, detector_partner) && !getSignalNormality(agents, presenter_partner, presenter)) { // Rule 6
                            updateAgentPairs(agents, presenter, presenter_partner, detector, detector_partner);
                        }
                    }
                }
            }
        }
    }

    // Agents' interaction and pairing dynamics
    void interactions(std::mt19937& generator, Agents& agents, unsigned short int const& n_presenters, std::vector<unsigned short int>& interactions_queue, std::vector<unsigned short int>& interaction_pairs)
    {
        // Shuffle interactions queue
        std::shuffle(interactions_queue.begin(), interactions_queue.end(), generator);

        // Shuffle interaction pairs
        std::shuffle(interaction_pairs.begin(), interaction_pairs.end(), generator);

        for (auto& interaction : interactions_queue) {
            // Decide interaction outcome
            decisionRules(agents, n_presenters, interaction, interaction_pairs.at(interaction));
        }

        // Update agents' taus
        for (auto& tau : agents.tau) {
            ++tau;
        }
    }

    // Base cellular frustration dynamics
    void cellularFrustration(unsigned short int const& seed, Agents& agents, unsigned short int const& n_presenters, unsigned int const& frustration_rounds, unsigned short int const& sample_rounds, unsigned short int const& n_samples, const std::vector<unsigned short int>& samples_queue, unsigned short int const& n_features, const std::vector<std::vector<float>>& data_set)
    {
        // Initialize generator used in dynamic loop
        std::mt19937 generator(seed);

        // Sample counter used to loop samples
        unsigned int sample_counter = 0;

        // Initialize interactions queue (indices = priority; elements = interaction pairs)
        std::vector<unsigned short int> interactions_queue(n_presenters);
        std::iota(interactions_queue.begin(), interactions_queue.end(), 0);

        // Initialize interaction pairs (indices = presenters' ids; elements = detectors' ids)
        std::vector<unsigned short int> interaction_pairs(n_presenters);
        std::iota(interaction_pairs.begin(), interaction_pairs.end(), n_presenters);

        // Main loop
        for (unsigned int round = 0; round < frustration_rounds; ++round) {
            // Loop through samples
            if (round % sample_rounds == 0) {
                changeSample(agents, n_presenters, n_samples, n_features, data_set.at(samples_queue.at(sample_counter++)), sample_counter);
            }

            // Loop through interactions between pairs of agents
            interactions(generator, agents, n_presenters, interactions_queue, interaction_pairs);
        }

        // Register taus on last round
        for (auto& id : agents.id) {
            ++agents.taus_map.at(id)[agents.tau.at(id)];
        }
    }

} // namespace cfm

#endif // CFMODEL_H
