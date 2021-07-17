
#ifndef AURORA_PARAMS_HPP
#define AURORA_PARAMS_HPP

#include "meta-cmaes/aurora_compilation_variables.hpp"
#include "meta-cmaes/aurora_parameters_factory.hpp"

constexpr aurora::env::Env environment = aurora::get_env();
constexpr aurora::algo::Algo algorithm = aurora::get_algo();
typedef aurora::SpecificParams specific_params_t;
typedef aurora::DefaultParamsFactory<environment,
                                     specific_params_t>::default_params_t default_params_t;
                                
typedef aurora::ParamsAlgo<algorithm, default_params_t, specific_params_t> params_t;
typedef default_params_t BottomParams;//


#endif