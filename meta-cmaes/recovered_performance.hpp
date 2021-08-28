
#ifndef RECOVERED_PERFORMANCE_HPP
#define RECOVERED_PERFORMANCE_HPP

#include <meta-cmaes/global.hpp>

#if CMAES_CHECK()
#include <meta-cmaes/cmaescheck_typedefs.hpp>
typedef CMAESCHECKParams BottomParams;
#endif

namespace sferes
{

    namespace fit
    {

        template <typename Phen>
        struct RecoveredPerformance
        {
            static std::vector<double> _eval_basefeatures(const Phen &indiv)
            {
                // copy of controller's parameters
                std::vector<double> _ctrl;
                _ctrl.clear();

                for (size_t i = 0; i < 24; i++)
                    _ctrl.push_back(indiv.gen().data(i));

                // launching the simulation
                auto robot = global::global_robot->clone();
                simulator_t simu(_ctrl, robot, 0, 1.0);
                simu.run(SIMU_TIME); // run simulation for the same amount of time as the bottom level, to keep function evals comparable
                std::vector<double> vec;
                simu.get_descriptor<rhex_dart::descriptors::DeltaFullTrajectory, std::vector<double>>(vec);
                return vec;
            }

            static float _eval_single_envir(const Phen &indiv, size_t world_option, size_t damage_option)
            {
                // copy of controller's parameters
                std::vector<double> _ctrl;
                _ctrl.clear();

                for (size_t i = 0; i < 24; i++)
                    _ctrl.push_back(indiv.gen().data(i));

#ifdef GRAPHIC
                std::string fileprefix = "video" + std::to_string(world_option) + std::to_string(damage_option);
#else
                std::string fileprefix = "";
#endif
#ifdef EVAL_ENVIR
                // launching the simulation
                auto robot = global::global_robot->clone();
                simulator_t simu(_ctrl, robot, world_option, 1.0);
#else
                auto robot = global::damaged_robots[damage_option]->clone();
                simulator_t simu(_ctrl, robot, world_option, 1.0, global::damage_sets[damage_option]);
#endif

                simu.run(SIMU_TIME); // run simulation for the same amount of time as the bottom level, to keep function evals comparable
                float fitness = simu.covered_distance();

                // these assume a behaviour descriptor of size 6.
                if (fitness < -1000)
                {
                    // this means that something bad happened in the simulation
                    // we do not kill the individual in the meta-map, but set fitness to zero and bd does not contribute
                    return 0.0; // will not count towards the sum
                    // do not update the descriptor !
                }
                else
                {
                    // update the meta-fitness
                    return fitness;
                }
            }
            static void _eval_taskmax(std::ostream &os, std::vector<boost::shared_ptr<Phen>> &individuals)
            {
                std::random_device rd;  //Will be used to obtain a seed for the random number engine
                std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
                std::cout << "will check " << individuals.size() << "individuals in random order" << std::endl;
#ifdef EVAL_ENVIR
                int damage = NULL; // will be ignored in eval_single_envir
                for (size_t world = 0; world < global::world_options.size(); ++world)
                {
                    os << "ENVIR \t" << world << std::endl;
#else
                size_t world = 0; // normal environment
                for (size_t damage = 0; damage < global::damage_sets.size(); ++damage)
                {
                    os << "DAMAGE \t" << damage << std::endl;
#endif

                    std::vector<boost::shared_ptr<Phen>> ids_left = individuals;
                    float best_so_far = -std::numeric_limits<float>::infinity();
                    while (ids_left.size() > 0)
                    {
                        //random choice
                        std::uniform_int_distribution<> dis(0, ids_left.size() - 1);
                        int index = dis(gen);
                        float val = _eval_single_envir(*ids_left[index], world, damage);
                        if (val > best_so_far)
                        {
                            best_so_far = val;
                        }
                        // remove index
                        ids_left.erase(ids_left.begin() + index);
                        os << best_so_far << std::endl;
                    }
                }
            }
#if NOT_AURORA()
            static void test_recoveredperformance(std::ostream &os, const boost::multi_array<boost::shared_ptr<Phen>, BEHAV_DIM> &archive)
            {
                float val = 0.0f;

                for (const bottom_indiv_t *k = archive.data(); k < (archive.data() + archive.num_elements()); ++k)
                {
                    if (*k)
                    {
#ifdef BASE_BEHAVS
                        std::vector<double> basebehavs = _eval_basefeatures(**k);
                        for (size_t i = 0; i < basebehavs.size(); ++i)
                        {
                            os << basebehavs[i] << " ";
                        }
#endif
                        val = _eval_all(**k);
#ifdef EVAL_ENVIR
                        val /= (float)global::world_options.size();
#else
                        val /= (float)global::damage_sets.size();
#endif
                        os << val << std::endl;
                    }
                }

                os << "END TEST META-INDIVIDUAL" << std::endl;
            }
            // assess maximal recovery for each damage separately
            static void test_max_recovery(std::ostream &os, const boost::multi_array<boost::shared_ptr<Phen>, BEHAV_DIM> &archive)
            {

                std::vector<bottom_indiv_t> individuals;
                for (const bottom_indiv_t *k = archive.data(); k < (archive.data() + archive.num_elements()); ++k)
                {
                    if (*k)
                    {
                        individuals.push_back(*k);
                    }
                }
                _eval_taskmax(os, individuals);
            }
#endif
            static void test_recoveredperformance(std::ostream &os, std::vector<boost::shared_ptr<Phen>> &archive)
            {
                float val = 0.0f;

                for (size_t k = 0; k < archive.size(); ++k)
                {

#ifdef BASE_BEHAVS
                    std::vector<double> basebehavs = _eval_basefeatures(**k);
                    for (size_t i = 0; i < basebehavs.size(); ++i)
                    {
                        os << basebehavs[i] << " ";
                    }
#endif
                    val = _eval_all(*archive[k]);
#ifdef EVAL_ENVIR
                    val /= (float)global::world_options.size();
#else
                    val /= (float)global::damage_sets.size();
#endif
                    os << val << std::endl;
                }
            }
            // assess maximal recovery for each damage separately
            static void test_max_recovery(std::ostream &os, std::vector<boost::shared_ptr<Phen>> &archive)
            {
                _eval_taskmax(os, archive);
            }
#ifdef EVAL_ENVIR
            static float _eval_all(const Phen &indiv)
            {
#ifdef PRINTING
                std::cout << "start evaluating " << global::world_options.size() << " environments" << std::endl;
#endif
                float val = 0.0f;
                for (size_t world_option : global::world_options)
                {
                    val += _eval_single_envir(indiv, world_option, 0);
                }
                return val;
            }
#else
            static float _eval_all(const Phen &indiv)
            {
#ifdef PRINTING
                std::cout << "start evaluating " << global::damage_sets.size() << " damage sets" << std::endl;
#endif
                float val = 0.0f;
                for (size_t i = 0; i < global::damage_sets.size(); ++i)
                {
                    // initilisation of the simulation and the simulated robot, robot morphology currently set to raised.skel only
                    float temp = _eval_single_envir(indiv, 0, i);
#ifdef GRAPHIC
                    std::cout << "performance " << temp << std::endl;
#endif
                    val += temp;
                }
                return val;
            }
#endif
        };

    } // namespace fit
} // namespace sferes

#endif
