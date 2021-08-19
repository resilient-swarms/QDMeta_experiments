
#ifndef FUNEXP_GLOBAL_HPP
#define FUNEXP_GLOBAL_HPP

#include <meta-cmaes/feature_vector_typedefs.hpp>
#include <set>
#if META()
#include <meta-cmaes/feature_map.hpp>
#include <meta-cmaes/database.hpp>
#include <meta-cmaes/params.hpp>
#include <sferes/ea/cmaes_interface.h>
#elif CMAES_CHECK()
#include <sferes/ea/cmaes_interface.h>
#endif
namespace global
{
    size_t nb_evals = 0;
    std::string outputdir;

    class RNG
    {
        std::random_device rd;
        std::mt19937 gen;
        long seed;

    public:
        RNG() {}
        RNG(long s)
        {
            gen = std::mt19937(s);
            seed = s;
        }
        /* int in [0,num-1] */
        int nextInt(size_t num)
        {
            std::uniform_int_distribution<unsigned> distrib(0, num - 1);
            return distrib(gen);
        }
        /* float in [0,1] */
        float nextFloat()
        {
            std::uniform_real_distribution<float> distrib(0.0f, 1.0f);
            return distrib(gen);
        }
    };
    RNG *rng;
    // sampling without replacement (see https://stackoverflow.com/questions/28287138/c-randomly-sample-k-numbers-from-range-0n-1-n-k-without-replacement)
    std::set<size_t> _pickSet(size_t N, size_t k, std::mt19937 &gen)
    {
        std::uniform_int_distribution<> dis(0, N - 1);
        std::set<size_t> elems;
        elems.clear();

        while (elems.size() < k)
        {
            elems.insert(dis(gen));
        }

        return elems;
    }

    std::set<size_t> _fullSet(size_t N)
    {
        std::set<size_t> elems;
        size_t k = 0;
        while (elems.size() < N)
        {
            elems.insert(k);
            ++k;
        }

        return elems;
    }

    std::set<size_t> _take_complement(std::set<size_t> full_set, std::set<size_t> sub_set)
    {
        std::set<size_t> diff;

        std::set_difference(full_set.begin(), full_set.end(), sub_set.begin(), sub_set.end(),
                            std::inserter(diff, diff.begin()));
        return diff;
    }

    // set the right condition
    enum ConditionType
    {
        meta = 0,
        gen = 1,
        cmaes_check = 2
    } condition;
#ifndef TEST
    void set_condition(const std::string &cond)
    {
        if (cond == "meta")
        {
            condition = ConditionType::meta;
            assert(META());
            assert(BEHAV_DIM == 2);
        }
        else if (cond == "gen")
        {
            condition = ConditionType::gen;
            assert(CONTROL() || AURORA());
#if CONTROL()
            assert(BEHAV_DIM == RASTRI_DIM);
#endif
        }
        else
        {
            throw std::runtime_error("condition " + cond + " not known");
        }
        std::cout << "done setting condition" << std::endl;
        ;
    }
#endif

#if CMAES_CHECK()
    size_t option_index = 0;
#endif
    std::vector<size_t> world_options; //

#if DIMENSION_TESTS()
    std::vector<std::set<size_t>> world_option_pairs; //
    void init_world(std::string seed)
    {
#if CMAES_CHECK()
        std::cout << "will do world_option " << global::option_index << std::endl;
#else
        std::ofstream ofs("dimension_options_" + seed + ".txt");
        std::seed_seq seed2(seed.begin(), seed.end());
        std::mt19937 gen(seed2);
        std::set<size_t> types = global::_pickSet(RASTRI_DIM, RASTRI_DIM / 2, gen);
        std::cout << "world options :" << std::endl;

        ofs << "{";
        for (size_t el : types)
        {
            size_t option = el; // world 0 has been removed that is why +1
#ifndef TEST
            world_options.push_back(option);
#endif
            std::cout << option << ", ";
            ofs << option << ", ";
        }
        ofs << "}";
        std::cout << std::endl;
#ifdef TEST
        types = global::_fullSet(20); // we will select 20 random perturbations
        ofs << "test:" << std::endl;
        ofs << "{";
        for (size_t i: types ) 
        {
            world_options.push_back(i);
            std::set<size_t> pair = global::_pickSet(RASTRI_DIM, 2, gen);// each perturbation is a 2-combinations of dimensions
            world_option_pairs.push_back(pair); // world 0 has been removed that is why +1
            std::cout << "{";
            ofs << "{";
            for( size_t j: pair)
            {
                std::cout << j << ", ";
                ofs << j << ", ";
            }
            std::cout << "}";
            ofs << "}";
        }
        ofs << "}";
        std::cout << std::endl;
#endif
#endif
    }
#elif TRANSLATION_TESTS()
    void init_world(std::string seed)
    {
        rng = new RNG((long)stoi(seed));
#if CMAES_CHECK()
        std::cout << "will do world_option " << global::option_index << std::endl;
#else
        std::ofstream ofs("dimension_options_" + seed + ".txt");
        std::seed_seq seed2(seed.begin(), seed.end());
        std::mt19937 gen(seed2);
        std::set<size_t> types = global::_fullSet(8);
        std::cout << "world options :" << std::endl;

        ofs << "{";
        for (size_t el : types)
        {
            size_t option = el; // world 0 has been removed that is why +1
#ifndef TAKE_COMPLEMENT
            world_options.push_back(option);
            world_options.push_back(option); // two of each type
#endif
            std::cout << option << ", ";
            ofs << option << ", ";
        }
        ofs << "}";
        std::cout << std::endl;
#ifdef TAKE_COMPLEMENT
#warning "make sure you implement this separately in the recovered_peformance"
#endif

#endif
    }
#else
    // no perturbations
    void init_world(std::string seed)
    {
    }

#endif

#if CMAES_CHECK()
    cmaes_t evo;
#endif
#if META()

    cmaes_t evo;
    // will use first-in-first-out queue such that latest DATABASE_SIZE individuals are maintained
    typedef EvoFloatDataEntry data_entry_t;
    typedef CircularBuffer<BottomParams::MAX_DATABASE_SIZE, data_entry_t> database_t;
    database_t database;

#endif
} // namespace global

#endif
