#ifndef BOTTOM_TYPEDEFS_HPP
#define BOTTOM_TYPEDEFS_HPP



#include <rhex_dart/descriptors.hpp>
#include <sferes/eval/eval.hpp>

#include <meta-cmaes/params.hpp>


#include <meta-cmaes/global.hpp>

#include <boost/serialization/vector.hpp> // serialising database vector

#include <boost/serialization/array.hpp>

#include <boost/fusion/container/vector.hpp>

//#include <boost/circular_buffer.hpp>
//#include <meta-cmaes/circular_buffer_serialisation.hpp>
#include <meta-cmaes/sampled.hpp>
#include <stdexcept>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// BOTTOM
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// // bottom-level typedefs
//typedef sferes::eval::Eval<BottomParams> bottom_eval_t;

typedef sferes::gen::Sampled<24, BottomParams> bottom_gen_t; // 24 parameters for our controller
typedef size_t bottom_gen_data_t;                            // sampled data type is based on unsigned ints
typedef boost::fusion::vector<> base_safe_t;
typedef boost::fusion::vector<rhex_dart::descriptors::DutyCycle, rhex_dart::descriptors::BodyOrientation, rhex_dart::descriptors::AvgCOMVelocities> base_desc_t;
typedef rhex_controller::RhexControllerBuehler base_controller_t;
typedef rhex_dart::RhexDARTSimu<rhex_dart::safety<base_safe_t>, rhex_dart::desc<base_desc_t>> simulator_t;
// note to self:
// adding rhex_dart::rhex_control<base_controller_t> as first argument to Simu type seems to fail

#endif
