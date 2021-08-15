
#ifndef AURORA_PARAMS_FUNEXP_HPP
#define AURORA_PARAMS_FUNEXP_HPP

namespace aurora
{
    namespace env
    {
        template <typename SpecificParams>
        struct ParamsFun
        {
            SFERES_CONST int
                min_update_period = 10;
            SFERES_CONST int
                max_update_period = 15000;
            SFERES_CONST int update_exponential_coefficient = 10;

            SFERES_CONST int update_container_period = SpecificParams::update_container_period;
            SFERES_CONST size_t
                image_width = 50;
            SFERES_CONST size_t
                image_height = 50;
            SFERES_CONST int
                resolution = 10000; // influences l; 30 bins per dimension
            SFERES_CONST int
                update_frequency = -1; // -2 means exponentially decaying update frequency
            SFERES_CONST size_t
                times_downsample = 4; // for taking the image

            SFERES_CONST bool use_colors = SpecificParams::use_colors;
            SFERES_CONST bool use_videos = SpecificParams::use_videos;
            SFERES_CONST aurora::env::Env env = SpecificParams::env;
            SFERES_CONST aurora::algo::Algo algo = SpecificParams::algo;
            SFERES_CONST aurora::EncoderType encoder_type = SpecificParams::encoder_type;
            SFERES_CONST bool do_consider_bumpers_in_obs_for_maze = SpecificParams::do_consider_bumpers_in_obs_for_maze;

            SFERES_CONST int batch_size = 20000;
            SFERES_CONST int nb_epochs = 100; // TO CHANGE

            static int step_measures;

            struct fit_data
            {
                static tbb::mutex sdl_mutex;
            };

            static inline int get_one_obs_size()
            {
                if (env == aurora::env::Env::Fun)
                { // In this case consider the Ground truth = (r,theta)
                    return RASTRI_DIM;
                }
            }

            static inline bool does_encode_sequence()
            {
                return (encoder_type == aurora::EncoderType::lstm_ae) || (encoder_type == aurora::EncoderType::conv_seq_ae);
            }

            struct nov
            {
                static double l;
                static bool use_fixed_l;
                SFERES_CONST double coefficient_proportional_control_l = SpecificParams::coefficient_proportional_control_l;
                SFERES_CONST double k = 15;
                SFERES_CONST double eps = 0.1;
            };

            struct pop
            {
                // size of a batch
                SFERES_CONST size_t init_size = 2000; // for eval_parallel
                SFERES_CONST size_t size = 400;       // note: this looks like double the other conditions but is the same due to incrementing with +=2 in quality_diversity.hpp vs +=1 in map_elites.hpp and cvt_map_elites.hpp
                SFERES_CONST size_t nb_gen = 250001;
                SFERES_CONST size_t dump_period = 100;
                SFERES_CONST size_t dump_period_aurora = 100;
            };

            struct evo_float
            {
                // we choose the polynomial mutation type
                SFERES_CONST sferes::gen::evo_float::mutation_t mutation_type = sferes::gen::evo_float::gaussian;
                // we choose the polynomial cross-over type
                SFERES_CONST sferes::gen::evo_float::cross_over_t cross_over_type = sferes::gen::evo_float::no_cross_over;
                // the mutation rate of the real-valued vector
                SFERES_CONST float mutation_rate = 0.1f;
                // the cross rate of the real-valued vector
                SFERES_CONST float cross_rate = 0.0f;
                // // a parameter of the polynomial mutation
                // SFERES_CONST float eta_m = 15.0f;
                // // a parameter of the polynomial cross-over
                // SFERES_CONST float eta_c = 10.0f;
                SFERES_CONST float sigma = 0.05f;
            };

            struct parameters
            {
                SFERES_CONST float min = 0.0f;
                SFERES_CONST float max = 1.0f;
            };

            // simulation time
            struct simu
            {
                SFERES_CONST size_t time = 5;
            };
            struct selector
            {
                SFERES_CONST float proba_picking_selector_1{0.5f};
            };

            struct qd
            {
                SFERES_CONST size_t behav_dim = SpecificParams::behav_dim;
            };
            struct lstm
            {
                SFERES_CONST size_t latent_size_per_layer = SpecificParams::lstm_latent_size_per_layer;
                SFERES_CONST size_t number_layers = SpecificParams::lstm_number_layers;
            };

            struct stat
            {
                SFERES_CONST size_t save_images_period = 5000;
                SFERES_CONST size_t period_saving_individual_in_population = 50;
            };

            struct taxons
            {
                //                SFERES_CONST int nb_max_policies = 5000;
                SFERES_CONST int Q = 5;
            };

            struct vat
            {
                SFERES_CONST float resolution_multiplicative_constant = 25.f;
            };
        };

        template <typename SpecificParams>
        double ParamsFun<SpecificParams>::nov::l;
        template <typename SpecificParams>
        bool ParamsFun<SpecificParams>::nov::use_fixed_l;
        template <typename SpecificParams>
        tbb::mutex ParamsFun<SpecificParams>::fit_data::sdl_mutex;
        template <typename SpecificParams>
        int ParamsFun<SpecificParams>::step_measures = 10;
    } // namespace env
} // namespace aurora

#endif //AURORA_PARAMS_RHEX_HPP
