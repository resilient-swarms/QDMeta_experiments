
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
                resolution = 900; // influences l; 30 bins per dimension
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
                SFERES_CONST size_t init_size = 400; // for eval_parallel
                SFERES_CONST size_t size = 80;// note: this looks like double the other conditions but is the same due to incrementing with +=2 in quality_diversity.hpp vs +=1 in map_elites.hpp and cvt_map_elites.hpp
                SFERES_CONST size_t nb_gen = 50001;
                SFERES_CONST size_t dump_period = 100;
                SFERES_CONST size_t dump_period_aurora = 100;
            };

            struct sampled
            {
                SFERES_ARRAY(float, values, 0.00f, 0.025f, 0.05f, 0.075f, 0.10f, 0.125f, 0.15f, 0.175f,
                             0.20f, 0.225f, 0.25f, 0.275f, 0.30f, 0.325f, 0.35f,
                             0.375f, 0.40f, 0.425f, 0.45f, 0.475f, 0.50f, 0.525f,
                             0.55f, 0.575f, 0.60f, 0.625f, 0.65f, 0.675f, 0.70f,
                             0.725f, 0.75f, 0.775f, 0.80f, 0.825f, 0.85f, 0.875f,
                             0.90f, 0.925f, 0.95f, 0.975f, 1.0f);

                SFERES_CONST float mutation_rate = 0.125f; //1 divided by number of genes
                SFERES_CONST float cross_rate = 0.00f;
                SFERES_CONST bool ordered = true; //increment or decrement rather than random selection
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
        double ParamsRHex<SpecificParams>::nov::l;
        template <typename SpecificParams>
        bool ParamsRHex<SpecificParams>::nov::use_fixed_l;
        template <typename SpecificParams>
        tbb::mutex ParamsRHex<SpecificParams>::fit_data::sdl_mutex;
        template <typename SpecificParams>
        int ParamsRHex<SpecificParams>::step_measures = 10;
    } // namespace env
} // namespace aurora

#endif //AURORA_PARAMS_RHEX_HPP
