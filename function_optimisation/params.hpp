#ifndef FUNEXP_PARAMS_HPP
#define FUNEXP_PARAMS_HPP

#include <sferes/phen/parameters.hpp>
#include <sferes/fit/fitness.hpp>

#include <sferes/gen/evo_float.hpp>
#include <meta-cmaes/sampled.hpp>

using namespace sferes;
using namespace sferes::gen::evo_float;



/* params for the bottom-level map */
struct BottomParams
{
#if META()
    static const size_t MAX_DATABASE_SIZE = 500000;
    static const size_t bottom_epochs = 5;
#endif
    // grid properties, discretise 3 dimensions into 10 bins each
    struct ea
    {
#ifdef CVT_ME
        SFERES_CONST size_t number_of_clusters = 10000;
        SFERES_CONST size_t number_of_dimensions = BEHAV_DIM;
        typedef boost::array<double, number_of_dimensions> point_t;
        static std::vector<point_t> centroids;
#else
        SFERES_CONST size_t behav_dim = 2;
        SFERES_ARRAY(size_t, behav_shape, 100,100); // 900 cells for each bottom-level map
#endif
        SFERES_CONST float epsilon = 0.00;
    };

    struct evo_float
    {
        // we choose the polynomial mutation type
        SFERES_CONST mutation_t mutation_type = gaussian;
        // we choose the polynomial cross-over type
        SFERES_CONST cross_over_t cross_over_type = no_cross_over;
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

    struct pop
    {
#if CONTROL()
        // number of generations
        SFERES_CONST unsigned nb_gen = 50000; // at most 20,000 evaluations for each of 500 meta-generations for meta-learning --> 10M evals (take a bit more just in case there is time enough)
        // how often should the result file be written (here, each 250 generations == 100,000 ; on 16-cores is every 1-2 hours)
        SFERES_CONST int dump_period = 100; //20
#endif
        // NOTE: multiply size by 2 to obtain the actual batch ! !
        SFERES_CONST unsigned size = 200;
        // NOTE: do NOT multiply by 2 to get initial size
        SFERES_CONST unsigned init_size = 2000;
        ; //initial population for all maps
        SFERES_CONST int initial_aleat = 1;
    };

    // parameter limits between 0 and 1  (scale to [-5.12,5.12] in rastrigin function)
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
};


#if META()
/* params for the top-level map */
struct CMAESParams
{
    // grid properties, discretise 3 dimensions into 10 bins each
    // struct ea {
    //     SFERES_CONST size_t behav_dim = 2;
    //     SFERES_ARRAY(size_t, behav_shape, 4, 4); // 16 cells based on two meta-descriptors
    //     SFERES_CONST float epsilon = 0.00;
    // };
    static constexpr float percentage_evaluated = 0.10f;
    struct evo_float // not used, but needed for compilation;
    {
        // we choose the polynomial mutation type
        SFERES_CONST mutation_t mutation_type = gaussian;
        // we choose the polynomial cross-over type
        SFERES_CONST cross_over_t cross_over_type = no_cross_over;
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

    // save map every 50 iterations
    struct pop
    {
        SFERES_CONST unsigned nb_gen = 100001; // huge overestimate, will hit max_evals before that
        SFERES_CONST int dump_period = 20;    //
        SFERES_CONST int size = META_POP_SIZE; // number of maps
        SFERES_CONST int initial_aleat = 1;
        SFERES_CONST unsigned max_evals = 100000000; //
    };

    struct parameters
    {
#if FEATUREMAP == NONLINEAR
        SFERES_CONST float min = -1.0f;
#else
        SFERES_CONST float min = 0.0f;
#endif
        SFERES_CONST float max = 1.0f;
    };
};
#endif
#endif
