#ifndef CFMODEL_H
#define CFMODEL_H

#include "utils.h"

namespace cfm
{

    // Agents' properties
    struct Agents
    {
        std::vector<unsigned short int> id;

        std::vector<unsigned short int> subtype;

        // Id of partner agent
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
        for (int i = 0; i < agents.id.size(); ++i) {
            agents.match.at(i) = -1;
        }
    }

    // Initialize agents' properties
    Agents initAgents(int const n_agents)
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

        for (int i = 0; i < n_agents; ++i) {
            agents.id.at(i) = i;

            if (agents.id.at(i) < n_agents / 2) {
                // Different subtypes for each half of presenters
                if (agents.id.at(i) < n_agents / 4) {
                    agents.subtype.at(i) = 1;
                    agents.global_list.at(i) = {1, 2};
                } else {
                    agents.subtype.at(i) = 2;
                    agents.global_list.at(i) = {2, 1};
                }
            } else {
                // Different subtypes for each half of detectors
                if (agents.id.at(i) < n_agents * 3/4) {
                    agents.subtype.at(i) = 1;
                    agents.signal.at(i) = 1;
                } else {
                    agents.subtype.at(i) = 2;
                    agents.signal.at(i) = 2;
                }
            }

            // Unpaired agents
            resetAgentsMatch(agents);
        }

        return agents;
    }

} // namespace cfm

#endif // CFMODEL_H
