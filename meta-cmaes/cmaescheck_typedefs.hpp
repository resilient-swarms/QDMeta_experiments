#ifndef CMAES_PARAMS_HPP
#define CMAES_PARAMS_HPP

#include <sferes/phen/parameters.hpp>
#include <sferes/fit/fitness.hpp>

#include <sferes/gen/evo_float.hpp>
#include <sferes/ea/ea.hpp>



#include <boost/serialization/vector.hpp> // serialising database vector

#include <boost/serialization/array.hpp>

#include <boost/fusion/container/vector.hpp>


#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <rhex_dart/descriptors.hpp>
#include <Eigen/Dense>


#include <meta-cmaes/global.hpp>


//#include <boost/circular_buffer.hpp>
//#include <meta-cmaes/circular_buffer_serialisation.hpp>
#include <meta-cmaes/feature_vector_typedefs.hpp>

#include <sferes/eval/eval.hpp>



using namespace sferes;
using namespace sferes::gen::evo_float;

/* params for CMAES CHECK (evolution or test phase) */
struct CMAESCHECKParams
{
    // grid properties, discretise 3 dimensions into 10 bins each
    // struct ea {
    //     SFERES_CONST size_t behav_dim = 2;
    //     SFERES_ARRAY(size_t, behav_shape, 4, 4); // 16 cells based on two meta-descriptors
    //     SFERES_CONST float epsilon = 0.00;
    // };
      // our values for each gait parameter can take on any one of these    ????????????????????

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
        SFERES_CONST float sigma = 0.05;
    };

    // save map every 50 iterations
    struct pop
    {
        SFERES_CONST unsigned nb_gen = 501;  
        SFERES_CONST int dump_period = 50;    
        SFERES_CONST int size = 5;           
        SFERES_CONST unsigned max_evals = 2600;
    };

    // parameter limits between 1.0 and 2.0  ( avoids negative weights ! )
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



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// BOTTOM
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// // bottom-level typedefs
//typedef sferes::eval::Eval<BottomParams> bottom_eval_t;typedef sferes::gen::EvoFloat<NUM_GENES, CMAESParams> gen_t;
typedef sferes::gen::EvoFloat<24, CMAESCHECKParams> bottom_gen_t; // 24 parameters for our controller
typedef size_t bottom_gen_data_t;                            // sampled data type is based on unsigned ints
typedef boost::fusion::vector<rhex_dart::safety_measures::TurnOver> base_safe_t;
typedef boost::fusion::vector<rhex_dart::descriptors::DeltaFullTrajectory> base_desc_t;
typedef rhex_controller::RhexControllerBuehler base_controller_t;
typedef rhex_dart::RhexDARTSimu<rhex_dart::safety<base_safe_t>, rhex_dart::desc<base_desc_t>> simulator_t;
// note to self:
// adding rhex_dart::rhex_control<base_controller_t> as first argument to Simu type seems to fail



#endif
