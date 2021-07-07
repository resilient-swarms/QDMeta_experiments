#ifndef AURORA_ENVIR_FACTORY_HPP
#define AURORA_ENVIR_FACTORY_HPP

#include <meta-cmaes/bottom_typedefs.hpp>
#include <meta-cmaes/aurora_compilation_variables.hpp>
#include <meta-cmaes/control_typedefs.hpp>

namespace aurora {
    namespace env {
        template<Env, typename Params>
        struct Environment {};

        template<typename Params>
        struct Environment<aurora::env::Env::RHex, Params> {
            typedef Params param_t;
            typedef sferes::fit::FitBottom<Params> fit_t;
            typedef bottom_gen_t gen_t;
            typedef sferes::phen::Parameters<gen_t, fit_t, Params> phen_t;
        };
    }
}

#endif //AURORA_ENVIRONMENTS_FACTORY_HPP
