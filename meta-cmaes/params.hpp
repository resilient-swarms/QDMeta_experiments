#ifndef META_CMAES_PARAMS_HPP
#define META_CMAES_PARAMS_HPP

#include <sferes/phen/parameters.hpp>
#include <sferes/fit/fitness.hpp>

#include <sferes/gen/evo_float.hpp>
#include <meta-cmaes/sampled.hpp>

using namespace sferes;
using namespace sferes::gen::evo_float;

#if AURORA()

struct BottomParams
{
    SFERES_CONST size_t update_period = 50;
    SFERES_CONST float K = 10.0;// arbitrary value to have a specific "resolution"; we get 200K + solutions with K=60,000. 7K solutions with K=1200
    struct nov
    {
        static double l;// novelty threshold
        SFERES_CONST double k = 15;
        SFERES_CONST double eps = 0.1;
    };

    // TODO: move to a qd::
    struct pop
    {
        // size of a batch
        SFERES_CONST size_t size = 200;
        SFERES_CONST size_t nb_gen = 250001;
        SFERES_CONST size_t dump_period = 200;
    };
    //   struct parameters {
    //     // used to avoid really degenerated experiments with trajectories not doing anything.
    //     SFERES_CONST float min = 0.1;
    //     SFERES_CONST float max = 0.9;
    //   };
    // our values for each gait parameter can take on any one of these
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
    // struct evo_float
    // {
    //     SFERES_CONST float cross_rate = 0.0f;
    //     SFERES_CONST float mutation_rate = 0.1f;
    //     SFERES_CONST float eta_m = 10.0f;
    //     SFERES_CONST float eta_c = 10.0f;
    //     SFERES_CONST mutation_t mutation_type = polynomial;
    //     SFERES_CONST cross_over_t cross_over_type = sbx;
    // };
    struct qd
    {
        SFERES_CONST size_t dim = BEHAV_DIM;
        SFERES_CONST size_t behav_dim = BEHAV_DIM;
    };
        // parameter limits between 0 and 1
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

#else

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
        SFERES_CONST size_t number_of_clusters = 4096;
        SFERES_CONST size_t number_of_dimensions = BEHAV_DIM;
        typedef boost::array<double, number_of_dimensions> point_t;
        static std::vector<point_t> centroids;
#else
        SFERES_CONST size_t behav_dim = 4;
        SFERES_ARRAY(size_t, behav_shape, 8,8,8,8); // 4096 cells for each bottom-level map
#endif
        SFERES_CONST float epsilon = 0.00;
    };

    // our values for each gait parameter can take on any one of these    ????????????????????
    struct sampled
    {
        SFERES_ARRAY(float, values, 0.00, 0.025, 0.05, 0.075, 0.10, 0.125, 0.15, 0.175,
                     0.20, 0.225, 0.25, 0.275, 0.30, 0.325, 0.35,
                     0.375, 0.40, 0.425, 0.45, 0.475, 0.50, 0.525,
                     0.55, 0.575, 0.60, 0.625, 0.65, 0.675, 0.70,
                     0.725, 0.75, 0.775, 0.80, 0.825, 0.85, 0.875,
                     0.90, 0.925, 0.95, 0.975, 1);

        SFERES_CONST float mutation_rate = 0.05f;
        SFERES_CONST float cross_rate = 0.00f;
        SFERES_CONST bool ordered = false;
    };

    struct pop
    {
#if CONTROL()
        // number of generations
        SFERES_CONST unsigned nb_gen = 25000; // at most 20,000 evaluations for each of 500 meta-generations for meta-learning --> 10M evals (take a bit more just in case there is time enough)
        // how often should the result file be written (here, each 250 generations == 100,000 ; on 16-cores is every 1-2 hours)
        SFERES_CONST int dump_period = 250; //20
#endif
        // NOTE: multiply size by 2 to obtain the actual batch ! !
        SFERES_CONST unsigned size = 200; //---> 400 individuals; note this is the size per map, for each bottom generation (total of 10,000 evals for bottom-level)
        //  --> leads to a total of at most 20,000 evaluations per meta-generation (400*25 + 0.10*4,096*25) for the environments case, and at most 1/1 ratio
        // filling up all or even half of the cells is quite unlikely though , so not too many worries for the damage case !

        // NOTE: do NOT multiply by 2 to get initial size
        SFERES_CONST unsigned init_size = 2000;
        ; //initial population for all maps
        SFERES_CONST int initial_aleat = 1;
    };

    // parameter limits between 0 and 1
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
#endif

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
        SFERES_CONST unsigned nb_gen = 100001; // overestimate, will hit max_evals before that
        SFERES_CONST int dump_period = 200;    //
        SFERES_CONST int size = META_POP_SIZE; // number of maps
        SFERES_CONST int initial_aleat = 1;
        SFERES_CONST unsigned max_evals = 100000000; //100M is equal to the bottomparams: 400*250000
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
