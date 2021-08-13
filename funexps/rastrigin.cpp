
#include "meta-cmaes/experiment_defines.hpp"

#include <boost/random.hpp>
#include <iostream>
#include <mutex>
#include <cstdlib>

#include <sferes/gen/evo_float.hpp>
//#include <sferes/gen/sampled.hpp>
#include <sferes/modif/dummy.hpp>
#include <sferes/phen/parameters.hpp>

#include <sferes/stat/pareto_front.hpp>

#include <chrono>

#include <sferes/stc.hpp>
#include <meta-cmaes/global.hpp>

#if META()
#include <meta-cmaes/meta-CMAES.hpp>
#include <meta-cmaes/stat_maps.hpp>
#include <meta-cmaes/stat_pop.hpp>
#include <meta-cmaes/params.hpp>
#include <meta-cmaes/parameter_control.hpp>
#include <meta-cmaes/bottom_typedefs.hpp>
#elif CMAES_CHECK()
#include <meta-cmaes/cmaescheck_fitness.hpp>
#include <meta-cmaes/cmaes.hpp>
#include <meta-cmaes/stat_bestgenotype.hpp>
typedef boost::fusion::vector<sferes::stat::BestGenotype<phen_t, CMAESCHECKParams>> stat_t;
typedef modif::Dummy<> modifier_t;
typedef sferes::ea::Cmaes<phen_t, eval_t, stat_t, modifier_t, CMAESCHECKParams> ea_t;
#elif AURORA()
#include <aurora/aurora.hpp>
double Params::nov::l = 0.01;
#include <meta-cmaes/bottom_typedefs.hpp>
#else
#include <meta-cmaes/control_typedefs.hpp>

#include <sferes/ea/ea.hpp>

#ifdef CVT_ME
#include <meta-cmaes/cvt_utils.hpp>
typedef boost::fusion::vector<sferes::stat::Map<phen_t, BottomParams>> stat_t;

typedef modif::Dummy<> modifier_t;
typedef sferes::ea::CVTMapElites<phen_t, eval_t, stat_t, modifier_t, BottomParams> ea_t;

#else
#ifdef TEST
#include <meta-cmaes/stat_map.hpp>
#else
#include <modules/map_elites/stat_map.hpp>
#endif
#include <modules/map_elites/map_elites.hpp>
typedef boost::fusion::vector<sferes::stat::Map<phen_t, BottomParams>> stat_t;

typedef modif::Dummy<> modifier_t;
typedef sferes::ea::MapElites<phen_t, eval_t, stat_t, modifier_t, BottomParams> ea_t;
#include <meta-cmaes/bottom_typedefs.hpp>
#endif

#endif

#ifdef CVT_ME
std::vector<BottomParams::ea::point_t> BottomParams::ea::centroids;
#endif

#include <sferes/run.hpp>
#if META()
#include <meta-cmaes/top_typedefs.hpp>


BOOST_CLASS_EXPORT_IMPLEMENT(EpochAnnealing)
BOOST_CLASS_EXPORT_IMPLEMENT(EpochEndogenous)
BOOST_CLASS_EXPORT_IMPLEMENT(RL)
BOOST_CLASS_EXPORT_IMPLEMENT(RLController)

#endif

#if META()
#ifndef TEST
boost::shared_ptr<ParameterControl> init_parameter_control_additionalopts(long seed, std::string choice, const char *resultsdir)
{
    mutationlogfile = std::string(resultsdir) + std::string("/mutation_log.txt");
    epochslogfile = std::string(resultsdir) + std::string("/epochs_log.txt");
    rewardlogfile = std::string(resultsdir) + std::string("/reward_log.txt");
    if (choice == "b1p1")
    {
        return boost::make_shared<ParameterControl>(1.f, 1.f);
    }
    else if (choice == "b1p2")
    {
        return boost::make_shared<ParameterControl>(1.f, 2.f);
    }
    else if (choice == "b1p5")
    {
        return boost::make_shared<ParameterControl>(1.f, 5.f);
    }
    else if (choice == "b1p10")
    {
        return boost::make_shared<ParameterControl>(1.f, 10.f); //100%
    }
    else if (choice == "b2p1")
    {
        return boost::make_shared<ParameterControl>(2.f, 1.f);
    }
    else if (choice == "b2p2")
    {
        return boost::make_shared<ParameterControl>(2.f, 2.f);
    }
    else if (choice == "b2p5")
    {
        return boost::make_shared<ParameterControl>(2.f, 5.f);
    }
    else if (choice == "b2p10")
    {
        return boost::make_shared<ParameterControl>(2.f, 10.f); //100%
    }
    else if (choice == "b5p1")
    {
        return boost::make_shared<ParameterControl>(5.f, 1.f);
    }
    else if (choice == "b5p2")
    {
        return boost::make_shared<ParameterControl>(5.f, 2.f);
    }
    else if (choice == "b5p5")
    {
        return boost::make_shared<ParameterControl>(5.f, 5.f);
    }
    else if (choice == "b5p10")
    {
        return boost::make_shared<ParameterControl>(5.f, 10.f); //100%
    }
    else if (choice == "b10p1")
    {
        return boost::make_shared<ParameterControl>(10.f, 1.f);
    }
    else if (choice == "b10p2")
    {
        return boost::make_shared<ParameterControl>(10.f, 2.f);
    }
    else if (choice == "b10p5")
    {
        return boost::make_shared<ParameterControl>(10.f, 5.f);
    }
    else if (choice == "b10p10")
    {
        return boost::make_shared<ParameterControl>(10.f, 10.f); //100%
    }
    else if (choice == "epochannealing_b10p1")
    {
        return boost::make_shared<EpochAnnealing>(10.f, 1.f);
    }
    else if (choice == "epochendogeneous_b10p1")
    {
        return boost::make_shared<EpochEndogenous>(10.f, 1.f);
    }
    else if (choice == "epochrl_b10p1")
    {
        std::string parameter = "bottom_epochs";
        return boost::make_shared<RL>(seed, parameter, 10.f, 1.f, 1.f);
    }
    else if (choice == "epochrl_b2p1")
    {
        std::string parameter = "bottom_epochs";
        return boost::make_shared<RL>(seed, parameter, 2.f, 1.f, 1.f);
    }
    else if (choice == "epochrl_b2p10")
    {
        std::string parameter = "bottom_epochs";
        return boost::make_shared<RL>(seed, parameter, 2.f, 10.f, 1.f);
    }
    else
    {
        throw std::runtime_error("not expected argument for parameter control " + choice);
    }
}
#endif
#endif

using namespace sferes;

int main(int argc, char **argv)
{
    size_t index = 0;
    for (size_t i = 0; i < argc; ++i)
    {
        if (std::string(argv[i]) == "--d")
        {
            index = i + 1;
            break;
        }
    }
    global::outputdir = argv[index];
    long seed = atoi(argv[1]);
    std::srand(seed); //use experiment number as seed for random generator. mostly for Eigen
#ifdef CVT_ME
    BottomParams::ea::centroids = load_centroids(std::string(std::getenv("BOTS_DIR")) + "/include/meta-cmaes/centroids/centroids_" + std::to_string(BottomParams::ea::number_of_clusters) + "_" + std::to_string(BottomParams::ea::number_of_dimensions) + ".dat");
#endif
    ea_t ea;

#ifdef PARALLEL_RUN
    sferes::eval::init_shared_mem<sferes::eval::CSharedMem>();
#endif

#if CMAES_CHECK()
#ifdef EVAL_ENVIR
    global::envir_index = atoi(argv[2]);
    std::cout << "will do envir " << global::envir_index << std::endl;
#else
    global::damage_index = atoi(argv[2]);
    std::cout << "will do damage " << global::damage_index << std::endl;
#endif

#elif CONTROL() || AURORA()
#ifndef TEST
    global::set_condition(argv[2]);
#endif

#elif META()
#ifndef TEST
    sferes::eval::param_ctrl = init_parameter_control_additionalopts(seed, std::string(argv[2]), argv[index]);
#endif
#endif

    // initialisation of the perturbations to the function if any
    global::init_world(std::string(argv[1]));
    run_ea(argc, argv, ea);
    return 0;
}
