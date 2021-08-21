
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
        const std::vector<float> b_neg_range = {-0.50, 0.};
        const std::vector<float> b_pos_range = {0., 0.5};
        const std::vector<float> a_neg_range = {-1.10, -1.0};
        const std::vector<float> a_pos_range = {1.0, 1.10};
        template <typename Phen>
        struct RecoveredPerformance
        {

            static float _eval_single_envir(const Phen &indiv, size_t world_option)
            {
                // copy of controller's parameters
                std::vector<float> _ctrl;
                _ctrl.clear();

                for (size_t i = 0; i < RASTRI_DIM; i++)
                    _ctrl.push_back(RASTRI_MIN + RASTRI_RANGE * indiv.gen().data(i));

#ifdef GRAPHIC
                std::string fileprefix = "video" + std::to_string(world_option);
#else
                std::string fileprefix = "";
#endif

#if DIMENSION_TESTS()
#ifdef TEST
                float fitness = evaluate_rastrigin_dimensionpairs(_ctrl, global::world_option_pairs[world_option]);
#else
                float fitness = evaluate_rastrigin_dimension(_ctrl, world_option);
#endif
#elif TRANSLATION_TESTS()
#ifdef TEST
                float fitness = evaluate_rastrigin_translationtest(_ctrl, world_option);
#else
                float fitness = evaluate_rastrigin_translation(_ctrl, world_option);
#endif
#else
#warning "not supported perturbation type";
                float fitness = 0.0f;
#endif
                // these assume a behaviour descriptor of size 6.

                return MAXFIT + std::max(-MAXFIT, fitness);
            }

            static float
            evaluate_rastrigin_dimension(const std::vector<float> &x, size_t world_option)
            {
                float sum = 10.0f * (RASTRI_DIM - 1);
                float A = 10.0f;
                for (size_t i = 0; i < RASTRI_DIM; ++i)
                {
#ifdef GRAPHIC
                    std::cout << x[i] << " ";
#endif
                    if (i == world_option)
                    {
#ifdef GRAPHIC
                        std::cout << "ignoring dimension " << i << std::endl;
#endif
                        continue;
                    }
                    sum += (x[i] * x[i] - A * std::cos(2 * M_PI * x[i]));
                }
#ifdef GRAPHIC
                std::cout << "f(x)=" << -sum << std::endl;
#endif
                return -sum; //maximisation rather than minimisation
            }
            static float
            evaluate_rastrigin_dimensionpairs(const std::vector<float> &x, std::set<size_t> world_option_pair)
            {
                float sum = 10.0f * (RASTRI_DIM - 1);
                float A = 10.0f;
                for (size_t i = 0; i < RASTRI_DIM; ++i)
                {
#ifdef GRAPHIC
                    std::cout << x[i] << " ";
#endif
                    if (world_option_pair.find(i) != world_option_pair.end())
                    {
#ifdef GRAPHIC
                        std::cout << "ignoring dimension " << i << std::endl;
#endif
                        // subtract higher-frequency sine wave (misleading phase shift and no x^2 penalty
                        // --> different optima spread across the space)
                        // averages out to zero though ("noise" -> hopefully can ignore)
                        sum -= A * std::sin(6.0f * M_PI * x[i]);
                        continue;
                    }
                    sum += (x[i] * x[i] - A * std::cos(2 * M_PI * x[i]));
                }
#ifdef GRAPHIC
                std::cout << "f(x)=" << -sum << std::endl;
#endif
                return -sum; //maximisation rather than minimisation
            }
            static float negative_a()
            {
                float a_min = a_neg_range[0];
                float a_max = a_neg_range[1];
                return a_min + (a_max - a_min) * global::rng->nextFloat();
            }
            static float negative_b()
            {
                float b_min = b_neg_range[0];
                float b_max = b_neg_range[1];
                return b_min + (b_max - b_min) * global::rng->nextFloat();
            }
            static float positive_a()
            {
                float a_min = a_pos_range[0];
                float a_max = a_pos_range[1];
                return a_min + (a_max - a_min) * global::rng->nextFloat();
            }
            static float positive_b()
            {
                float b_min = b_pos_range[0];
                float b_max = b_pos_range[1];
                return b_min + (b_max - b_min) * global::rng->nextFloat();
            }
#if TRANSLATION_TESTS()
            static float evaluate_rastrigin_translationtest(std::vector<float> &x, size_t world_option)
            {
                
                // test set (see global.hpp)
                float a = global::get_test_a(world_option);
                float b = global::get_test_b(world_option);
                
                float sum = 10.0f * RASTRI_DIM;
#ifdef GRAPHIC
                std::cout << "translation with a=" << a << " b=" << b << std::endl;
#endif
                float A = 10.0f;
                for (size_t i = 0; i < RASTRI_DIM; ++i)
                {
#ifdef GRAPHIC
                    std::cout << "before: " << x[i] << std::endl;
#endif
                    x[i] = a * x[i] + b;

#ifdef GRAPHIC
                    std::cout << "after: " << x[i] << std::endl;
#endif
                    if (std::abs(x[i]) > RASTRI_MAX)
                    {
#ifdef GRAPHIC
                        std::cout << "not in bounds: stop " << std::endl;
#endif
                        return -MAXFIT;
                    }
                    sum += (x[i] * x[i] - A * std::cos(2 * M_PI * x[i]));
                }
#ifdef GRAPHIC
                std::cout << "f(x)=" << -sum << std::endl;
#endif
                return -sum; //maximisation rather than minimisation
            }
#endif
            static float evaluate_rastrigin_translation(std::vector<float> &x, size_t world_option)
            {
                float a, b;
                // a > 1: expand
                if (world_option == 0)
                {
                    a = negative_a();
                    b = negative_b();
                }
                else if (world_option == 1)
                {
                    a = negative_a();
                    b = positive_b();
                }
                else if (world_option == 2)
                {
                    a = positive_a();
                    b = negative_b();
                }
                else if (world_option == 3)
                {
                    a = positive_a();
                    b = positive_b();
                }
                // a < 1: shrink
                else if (world_option == 4)
                {
                    a = 1.0f / negative_a();
                    b = negative_b();
                }
                else if (world_option == 5)
                {
                    a = 1.0f / negative_a();
                    b = positive_b();
                }
                else if (world_option == 6)
                {
                    a = 1.0f / positive_a();
                    b = negative_b();
                }
                else if (world_option == 7)
                {
                    a = 1.0f / positive_a();
                    b = positive_b();
                }
                else
                {
                    throw std::runtime_error("can take 8 options only {0,..,7}");
                }
                float sum = 10.0f * RASTRI_DIM;
#ifdef GRAPHIC
                std::cout << "translation with a=" << a << " b=" << b << std::endl;
#endif
                float A = 10.0f;
                for (size_t i = 0; i < RASTRI_DIM; ++i)
                {
#ifdef GRAPHIC
                    std::cout << "before: " << x[i] << std::endl;
#endif
                    x[i] = a * x[i] + b;

#ifdef GRAPHIC
                    std::cout << "after: " << x[i] << std::endl;
#endif
                    if (std::abs(x[i]) > RASTRI_MAX)
                    {
#ifdef GRAPHIC
                        std::cout << "not in bounds: stop " << std::endl;
#endif
                        return -MAXFIT;
                    }
                    sum += (x[i] * x[i] - A * std::cos(2 * M_PI * x[i]));
                }
#ifdef GRAPHIC
                std::cout << "f(x)=" << -sum << std::endl;
#endif
                return -sum; //maximisation rather than minimisation
            }
            //             static float evaluate_rastrigin_noise(const std::vector<float> &x, size_t world_option)
            //             {
            //                 float sum = 10.0f * RASTRI_DIM;
            //                 std::vector<float> x = ctrl();
            // #ifdef GRAPHIC
            //                 std::cout << "translation with a=" << a << " b=" << b << std::endl;
            // #endif
            //                 float A = 10.0f;
            //                 for (size_t i = 0; i < RASTRI_DIM; ++i)
            //                 {
            // #ifdef GRAPHIC
            //                     std::cout << "before: " << x[i] << std::endl;
            // #endif
            //                     x[i] = a * x[i] + b;
            // #ifdef GRAPHIC
            //                     std::cout << "after: " << x[i] << std::endl;
            // #endif
            //                     sum += (x[i] * x[i] - A * std::cos(2 * M_PI * x[i]));
            //                 }
            // #ifdef GRAPHIC
            //                 std::cout << "f(x)=" << -sum << std::endl;
            // #endif
            //                 return -sum; //maximisation rather than minimisation
            //             }
            static void _eval_taskmax(std::ostream &os, std::vector<boost::shared_ptr<Phen>> &individuals)
            {
                std::random_device rd;  //Will be used to obtain a seed for the random number engine
                std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
                std::cout << "will check " << individuals.size() << "individuals in random order" << std::endl;
                int damage = NULL; // will be ignored in eval_single_envir
                for (size_t world = 0; world < global::world_options.size(); ++world)
                {
                    os << "ENVIR \t" << world << std::endl;

                    std::vector<boost::shared_ptr<Phen>> ids_left = individuals;
                    float best_so_far = -std::numeric_limits<float>::infinity();
                    while (ids_left.size() > 0)
                    {
                        //random choice
                        std::uniform_int_distribution<> dis(0, ids_left.size() - 1);
                        int index = dis(gen);
                        float val = _eval_single_envir(*ids_left[index], world);
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
                        val = _eval_all(**k);
                        val /= (float)global::world_options.size();
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

                    val = _eval_all(*archive[k]);
                    val /= (float)global::world_options.size();
                    os << val << std::endl;
                }
            }
            // assess maximal recovery for each damage separately
            static void
            test_max_recovery(std::ostream &os, std::vector<boost::shared_ptr<Phen>> &archive)
            {
                _eval_taskmax(os, archive);
            }

            static float _eval_all(const Phen &indiv)
            {
#ifdef PRINTING
                std::cout << "start evaluating " << global::world_options.size() << " environments" << std::endl;
#endif
                float val = 0.0f;
                for (size_t world_option : global::world_options)
                {
                    val += _eval_single_envir(indiv, world_option);
                }
                return val;
            }
        };

    } // namespace fit
} // namespace sferes

#endif
