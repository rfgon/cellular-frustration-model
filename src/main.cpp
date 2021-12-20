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

    return 0;
}
