#define LATENT_SPACE_SIZE BEHAV_DIM

#include <meta-cmaes/experiment_defines.hpp>


#include <boost/random.hpp>
#include <iostream>
#include <mutex>
#include <cstdlib>
#include <chrono>

#include <sferes/stc.hpp>
#include <meta-cmaes/global.hpp>

#include <sys/stat.h>
#include <cstdint>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <unistd.h>

#include <Eigen/Core>
#include <Eigen/Eigenvalues>

#include <boost/serialization/map.hpp>

//#include <boost/process.hpp>
#include <boost/program_options.hpp>

#include <torch/torch.h>
#include <torch/script.h> // One-stop header.

#include <sferes/eval/parallel.hpp>
#include <sferes/gen/evo_float.hpp>
#include <sferes/modif/dummy.hpp>
#include <sferes/phen/parameters.hpp>
#include <sferes/run.hpp>
#include <sferes/stat/best_fit.hpp>
#include <sferes/stat/qd_container.hpp>
#include <sferes/stat/qd_selection.hpp>
#include <sferes/stat/qd_progress.hpp>

#include <sferes/fit/fit_qd.hpp>
#include <sferes/qd/container/archive.hpp>
#include <sferes/qd/container/kdtree_storage.hpp>
#include <sferes/qd/quality_diversity.hpp>
#include <sferes/qd/selector/value_selector.hpp>
#include <sferes/qd/selector/score_proportionate.hpp>


#include <meta-cmaes/aurora_compilation_variables.hpp>

#include <meta-cmaes/bottom_typedefs.hpp>

#include "modifier/network_loader_pytorch.hpp"
#include "modifier/dimensionality_reduction.hpp"
#include "modifier/container_update_hand_coded.hpp"

#include "stat/stat_projection.hpp"
#include "stat/stat_current_gen.hpp"
#include "stat/stat_model_autoencoder.hpp"
#include "stat/stat_modifier.hpp"
#include "stat/stat_successive_gt.hpp"

#include "algorithms/selector/variable_selector.hpp"
#include "algorithms/selector/surprise_value_selector.hpp"

#include "meta-cmaes/aurora_encoder_factory.hpp"
#include <meta-cmaes/aurora_parameters_factory.hpp>
#include <meta-cmaes/aurora_environments_factory.hpp>
#include <meta-cmaes/aurora_algorithms_factory.hpp>

namespace aurora
{

    struct Arguments
    {
        size_t number_threads;
        int step_measures;
    };

    void get_arguments(const boost::program_options::options_description &desc, Arguments &arg, int argc, char **argv)
    {
        // For the moment, only returning number of threads
        boost::program_options::variables_map vm;
        boost::program_options::parsed_options parsed = boost::program_options::command_line_parser(argc, argv).options(desc).allow_unregistered().run();

        boost::program_options::store(parsed, vm);
        boost::program_options::notify(vm);
        arg.number_threads = vm["number-threads"].as<size_t>();
    }
} // namespace aurora
typedef aurora::env::Environment<environment, params_t> env_t;
typedef env_t::phen_t phen_t;
namespace sferes
{
    namespace stat
    {
        template <>
        void QdContainer<phen_t, params_t>::show(std::ostream &os, size_t k)
        {
#ifdef TEST
#ifdef GRAPHIC // we are just interested in observing a particular individual
            _archive[k]->develop();
            float val = sferes::fit::RecoveredPerformance<phen_t>::_eval_all(_container[k]);
#else
#ifdef INDIVIDUAL_DAMAGE
            sferes::fit::RecoveredPerformance<phen_t>::test_max_recovery(os, _container);
#else
            sferes::fit::RecoveredPerformance<phen_t>::test_recoveredperformance(os, _container);
#endif

#endif
#endif
        }
    }
}

int main(int argc, char **argv)
{

    dbg::enable(dbg::tracing, "ea", true);
    dbg::attach_ostream(dbg::tracing, std::cout);
    airl::dbg::attach_ostream(airl::dbg::tracing, std::cout);
    airl::dbg::attach_ostream(airl::dbg::debug, std::cout);
    // airl::dbg::enable_all(airl::dbg::debug, true);
    airl::dbg::enable_all(airl::dbg::info, true);
    airl::dbg::enable_all(airl::dbg::debug, true);
    airl::dbg::enable_all(airl::dbg::tracing, true);
    airl::dbg::enable_level_prefix(true);
    airl::dbg::enable_time_prefix(true);

    std::cout << aurora::get_lp_norm() << std::endl;

    boost::program_options::options_description desc;
    aurora::Arguments arg{};

    constexpr int step_measures_default_value = 10;
    desc.add_options()("number-threads", boost::program_options::value<size_t>(), "Set Number of Threads")("step-measures", boost::program_options::value<int>(&arg.step_measures)->default_value(step_measures_default_value), "Step between two stored measures in Hard Maze environment");

    aurora::get_arguments(desc, arg, argc, argv);

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
    std::srand(seed);

    tbb::task_scheduler_init init(arg.number_threads);
    torch::set_num_threads(arg.number_threads);

    params_t::step_measures = arg.step_measures;
    params_t::nov::use_fixed_l = specific_params_t::use_fixed_l;

    static_assert(
        (params_t::encoder_type != aurora::EncoderType::lstm_ae) || (params_t::use_videos),
        "Use of LSTM AE => need for use_videos");

    typedef aurora::algo::AlgorithmFactory<algorithm, env_t>::algo_t algo_t;

    algo_t::update_parameters();

    typedef algo_t::ea_t ea_t;

    ea_t ea;

#ifdef TEST
#if CMAES_CHECK()
    global::damage_index = atoi(argv[2]);
    std::cout << "will do damage " << global::damage_index << std::endl;
#endif
#endif

    global::init_world(std::string(argv[1]));

    sferes::run_ea(argc, argv, ea, desc);

    return 0;
}
