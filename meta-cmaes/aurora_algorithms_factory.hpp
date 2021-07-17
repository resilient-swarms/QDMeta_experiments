

#ifndef AURORA_ALGORITHMS_FACTORY_HPP
#define AURORA_ALGORITHMS_FACTORY_HPP

#include "meta-cmaes/aurora_compilation_variables.hpp"

#include "algorithms/taxons/taxons_evolutionary_algorithm.hpp"

#include "algorithms/taxons/definitions_taxons.hpp"

#include "algorithms/hand_coded/hand_coded_qd.hpp"
#include "algorithms/hand_coded/novelty_search.hpp"




namespace aurora {
    namespace algo {

        template<Algo, typename Environment>
        struct AlgorithmFactory {};

        template<typename Environment>
        struct AlgorithmFactory<aurora::algo::Algo::aurora_curiosity, Environment> {
            typedef AuroraCuriosity<Environment, aurora::get_encoder_type(), aurora::get_lp_norm()> algo_t;
        };

        template<typename Environment>
        struct AlgorithmFactory<aurora::algo::Algo::aurora_novelty, Environment> {
            typedef AuroraNovelty<Environment, aurora::get_encoder_type(), aurora::get_lp_norm()> algo_t;
        };

        template<typename Environment>
        struct AlgorithmFactory<aurora::algo::Algo::aurora_surprise, Environment> {
            typedef AuroraSurprise<Environment, aurora::get_encoder_type(), aurora::get_lp_norm()> algo_t;
        };

        template<typename Environment>
        struct AlgorithmFactory<aurora::algo::Algo::aurora_nov_sur, Environment> {
            typedef AuroraNoveltySurprise<Environment, aurora::get_encoder_type(), aurora::get_lp_norm()> algo_t;
        };

        template<typename Environment>
        struct AlgorithmFactory<aurora::algo::Algo::aurora_uniform, Environment> {
            typedef AuroraUniform<Environment, aurora::get_encoder_type(), aurora::get_lp_norm()> algo_t;
        };

    }
}

#endif //AURORA_ALGORITHMS_FACTORY_HPP
