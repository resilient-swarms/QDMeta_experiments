

#ifndef AURORA_PARAMETERS_FACTORY_HPP
#define AURORA_PARAMETERS_FACTORY_HPP

#include "meta-cmaes/aurora_definitions_aurora.hpp"
#include "meta-cmaes/aurora_params_rhex.hpp"
#include "meta-cmaes/aurora_params_fun.hpp"

namespace aurora
{
  struct SpecificParams
  {

    SFERES_CONST aurora::env::Env env = aurora::get_env();
    SFERES_CONST aurora::algo::Algo algo = aurora::get_algo();
    SFERES_CONST aurora::EncoderType encoder_type = aurora::get_encoder_type();

    SFERES_CONST int behav_dim = LATENT_SPACE_SIZE;

    SFERES_CONST double coefficient_proportional_control_l = aurora::get_coefficient_proportional_control_l();
    SFERES_CONST int update_container_period = aurora::get_update_container_period();

    //#ifdef NUMBER_LAYERS_LSTM
    //        SFERES_CONST int number_layers = NUMBER_LAYERS_LSTM;
    //#endif
    //
    //#ifdef LATENT_SIZE_LAYER
    //        SFERES_CONST int latent_size_per_layer = LATENT_SIZE_LAYER;
    //#endif

    SFERES_CONST bool use_colors = aurora::get_use_colors();
    SFERES_CONST bool use_videos = aurora::get_use_videos();
    SFERES_CONST bool use_fixed_l = aurora::get_use_fixed_l();

    SFERES_CONST bool do_consider_bumpers_in_obs_for_maze = aurora::get_do_consider_bumpers_in_obs_for_maze();
    SFERES_CONST int lstm_latent_size_per_layer = aurora::get_lstm_latent_size_per_layer();
    SFERES_CONST int lstm_number_layers = aurora::get_lstm_number_layers();
  };

  template <env::Env, typename SpecificParameters>
  struct DefaultParamsFactory
  {
  };

  template <typename SpecificParameters>
  struct DefaultParamsFactory<env::Env::RHex, SpecificParameters>
  {
    typedef env::ParamsRHex<SpecificParameters> default_params_t;
  };

  template <typename SpecificParameters>
  struct DefaultParamsFactory<env::Env::Fun, SpecificParameters>
  {
    typedef env::ParamsFun<SpecificParameters> default_params_t;
  };

  template <algo::Algo, typename DefaultParameters, typename SpecificParameters>
  struct ParamsAlgo : public DefaultParameters
  {
  };
}

#endif //AURORA_PARAMETERS_FACTORY_HPP
