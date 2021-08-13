#ifndef FUNEXP_BOTTOM_TYPEDEFS_HPP
#define FUNEXP_BOTTOM_TYPEDEFS_HPP

#include <sferes/eval/eval.hpp>
#include <sferes/gen/evo_float.hpp>

#if AURORA()
#include <meta-cmaes/aurora_params.hpp>
#else
#include <meta-cmaes/params.hpp>
#endif

#include <meta-cmaes/global.hpp>

#include <boost/serialization/vector.hpp> // serialising database vector

#include <boost/serialization/array.hpp>

#include <boost/fusion/container/vector.hpp>

#ifdef PARALLEL_RUN
#include <meta-cmaes/eval_parallel.hpp>
#endif
#include <stdexcept>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// BOTTOM
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


typedef sferes::gen::EvoFloat<RASTRI_DIM, BottomParams> bottom_gen_t; 
typedef size_t bottom_gen_data_t;                            // sampled data type is based on unsigned ints
#ifdef PARALLEL_RUN
     typedef sferes::eval::CSharedMem shared_memory_t ;	
#endif
#endif
