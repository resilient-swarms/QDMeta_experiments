
#include <rhex_dart/experiment_defines.hpp>

#include <boost/random.hpp>
#include <iostream>
#include <mutex>
#include <cstdlib>

#include <sferes/gen/evo_float.hpp>
//#include <sferes/gen/sampled.hpp>
#include <sferes/modif/dummy.hpp>
#include <sferes/phen/parameters.hpp>

#include <sferes/stat/pareto_front.hpp>

#include <rhex_dart/rhex_dart_simu.hpp>
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

BOOST_CLASS_EXPORT_IMPLEMENT(MutationAnnealing)
BOOST_CLASS_EXPORT_IMPLEMENT(EpochAnnealing)
BOOST_CLASS_EXPORT_IMPLEMENT(BothAnnealing)
BOOST_CLASS_EXPORT_IMPLEMENT(MutationEndogenous)
BOOST_CLASS_EXPORT_IMPLEMENT(EpochEndogenous)
BOOST_CLASS_EXPORT_IMPLEMENT(BothEndogenous)
BOOST_CLASS_EXPORT_IMPLEMENT(RL)
BOOST_CLASS_EXPORT_IMPLEMENT(RLController)

#endif

#if NOT_CMAES_CHECK()
namespace sferes
{
    namespace gen
    {
        template <>
        void bottom_gen_t::mutate()
        {
            if (BottomParams::sampled::ordered)
            {
                for (size_t i = 0; i < _data.size(); ++i)
                {
#if META()
                    float mutation_rate = sferes::eval::param_ctrl->get_mutation_rate();
#else
                    float mutation_rate = BottomParams::sampled::mutation_rate;
#endif
                    if (misc::rand<float>() < mutation_rate)
                    {
                        if (misc::flip_coin())
                            _data[i] = std::max(0, (int)_data[i] - 1);
                        else
                            _data[i] = std::min((int)BottomParams::sampled::values_size() - 1,
                                                (int)_data[i] + 1);
                    }
                }
            }
            else
            {
#if META()
                float mutation_rate = sferes::eval::param_ctrl->get_mutation_rate();
#else
                float mutation_rate = BottomParams::sampled::mutation_rate;
#endif
                BOOST_FOREACH (size_t &v, _data)
                    if (misc::rand<float>() < mutation_rate)
                        v = misc::rand<size_t>(0, BottomParams::sampled::values_size());
                _check_invariant();
            }
            _check_invariant();
        }
    }
}
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
    else if (choice == "b1p1m1")
    {
        return boost::make_shared<ParameterControl>(1.f, 1.f, 1.f);
    }
    else if (choice == "b1p1m2")
    {
        return boost::make_shared<ParameterControl>(1.f, 1.f, 2.f);
    }
    else if (choice == "b1p1m4")
    {
        return boost::make_shared<ParameterControl>(1.f, 1.f, 4.f);
    }
    else if (choice == "b1p1m8")
    {
        return boost::make_shared<ParameterControl>(1.f, 1.f, 8.f); //100%
    }
    else if (choice == "epochannealing_b2p1")
    {

        return boost::make_shared<EpochAnnealing>(2.f, 1.f);
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
    else if (choice == "mutationannealing_b1p1m2")
    {
        return boost::make_shared<MutationAnnealing>(1.f, 1.f, 2.f);
    }
    else if (choice == "mutationannealing_b1p1m8")
    {
        return boost::make_shared<MutationAnnealing>(1.f, 1.f, 8.f);
    }
    else if (choice == "mutationendogeneous_b1p1m8")
    {
        return boost::make_shared<MutationEndogenous>(1.f, 1.f, 8.f);
    }
    else if (choice == "mutationrl_b1p1m8")
    {
        std::string parameter = "mutation_rate";
        return boost::make_shared<RL>(seed, parameter, 1.f, 1.f, 8.f);
    }
    else if (choice == "bothendogeneous_b10p1m8")
    {
        return boost::make_shared<BothEndogenous>(10.f, 1.f, 8.f);
    }
    else if (choice == "bothrl_b10p1m8")
    {
        std::string parameter = "both";
        return boost::make_shared<RL>(seed, parameter, 10.f, 1.f, 8.f);
    }
    else if (choice == "bothannealing_b10p1m8")
    {
        return boost::make_shared<BothAnnealing>(10.f, 1.f, 8.f);
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

    // initialisation of the simulation and the simulated robot, robot morphology currently set to raised.skel only
    global::init_simu(std::string(argv[1]), std::string(std::getenv("BOTS_DIR")) + "/share/rhex_models/SKEL/raised.skel");
    run_ea(argc, argv, ea);

    global::global_robot.reset();
    return 0;
}
