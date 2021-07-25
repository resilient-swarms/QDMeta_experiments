#define LATENT_SPACE_SIZE BEHAV_DIM

#include <rhex_dart/experiment_defines.hpp>


#include <boost/random.hpp>
#include <iostream>
#include <mutex>
#include <cstdlib>

#include <rhex_dart/rhex_dart_simu.hpp>
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

// replacing physics with fit_hexa
// #include "minimal_physics.hpp"

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


namespace sferes
{
    namespace gen
    {
        template <>
        void bottom_gen_t::mutate()
        {
            if (default_params_t::sampled::ordered)
            {
                for (size_t i = 0; i < _data.size(); ++i)
                {

                    float mutation_rate = default_params_t::sampled::mutation_rate;
                    if (misc::rand<float>() < mutation_rate)
                    {
                        if (misc::flip_coin())
                            _data[i] = std::max(0, (int)_data[i] - 1);
                        else
                            _data[i] = std::min((int)default_params_t::sampled::values_size() - 1,
                                                (int)_data[i] + 1);
                    }
                }
            }
            else
            {
                float mutation_rate = default_params_t::sampled::mutation_rate;
                BOOST_FOREACH (size_t &v, _data)
                    if (misc::rand<float>() < mutation_rate)
                        v = misc::rand<size_t>(0, default_params_t::sampled::values_size());
                _check_invariant();
            }
            _check_invariant();
        }
    } // namespace gen
} // namespace sferes

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

    typedef aurora::env::Environment<environment, params_t> env_t;
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

    global::init_simu(std::string(argv[1]), std::string(std::getenv("BOTS_DIR")) + "/share/rhex_models/SKEL/raised.skel");

    sferes::run_ea(argc, argv, ea, desc);

    return 0;
}
