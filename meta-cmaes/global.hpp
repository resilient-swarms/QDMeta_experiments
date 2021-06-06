
#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <rhex_dart/safety_measures.hpp>
#include <rhex_dart/descriptors.hpp>
#include <rhex_dart/rhex.hpp>
#include <meta-cmaes/feature_vector_typedefs.hpp>

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
    std::string outputdir;
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
        pos,
        tra,
        cmaes_check
    } condition;
#ifndef TEST
    void set_condition(const std::string &cond)
    {
        if (cond == "meta")
        {
            condition = ConditionType::meta;
            assert(META());
            assert(BEHAV_DIM == 4);
        }
        else if (cond == "pos")
        {
            condition = ConditionType::pos;
            assert(CONTROL());
            assert(BEHAV_DIM == 2);
        }
        else if (cond == "tra")
        {
            condition = ConditionType::tra;
            assert(CONTROL() || AURORA());
#if CONTROL()
            assert(BEHAV_DIM == 50);
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
    size_t damage_index = 0;
    size_t envir_index = 0; //
#endif
    const double BODY_LENGTH = .54;
    const double BODY_WIDTH = .39;
    const double BODY_HEIGHT = .139;
    size_t nb_evals = 0;
    std::shared_ptr<rhex_dart::Rhex> global_robot;

#if ENVIR_TESTS()
    std::vector<size_t> world_options;

    void init_world(std::string seed, std::string robot_file)
    {
#if CMAES_CHECK()
        // noe need for this vector world_options = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}; // all
        std::cout << "will do world_option " << global::envir_index << std::endl;

#else
        std::ofstream ofs("world_options_" + seed + ".txt");
        std::seed_seq seed2(seed.begin(), seed.end());
        std::mt19937 gen(seed2);
        std::set<size_t> types = global::_pickSet(9, 5, gen);
        std::cout << "world options :" << std::endl;

        ofs << "{";
        for (size_t el : types)
        {
            size_t option = el + 1; // world 0 has been removed that is why +1
#ifndef TAKE_COMPLEMENT
            world_options.push_back(option);
#endif
            std::cout << option << ", ";
            ofs << option << ", ";
        }
        ofs << "}";
        std::cout << std::endl;
#ifdef TAKE_COMPLEMENT
        types = global::_take_complement(global::_fullSet(9), types);
        ofs << "test:" << std::endl;
        ofs << "{";
        for (size_t el : types)
        {
            world_options.push_back(el + 1); // world 0 has been removed that is why +1
            std::cout << world_options.back() << ", ";
            ofs << world_options.back() << ", ";
        }
        ofs << "}";
        std::cout << std::endl;
#endif
#endif
    }
#endif

#if DAMAGE_TESTS()
    std::vector<std::shared_ptr<rhex_dart::Rhex>> damaged_robots;
    std::vector<std::vector<rhex_dart::RhexDamage>> damage_sets;
    // {rhex_dart::RhexDamage("leg_removal", "26")}
    void init_damage(std::string seed, std::string robot_file)
    {
        std::vector<std::string> damage_types = {"leg_removal", "blocked_joint", "leg_shortening", "passive_joint"};
#if CMAES_CHECK()
        std::set<size_t> types = {0, 1, 2, 3}; // all
        for (size_t el : types)
        {
            std::string damage_type = damage_types[el];
            for (size_t leg = 0; leg < 6; ++leg)
            {
                damage_sets.push_back({rhex_dart::RhexDamage(damage_type.c_str(), std::to_string(leg).c_str())});
            }
        }
        std::cout << "will do damage " << global::damage_index << ": " << damage_sets[global::damage_index][0].type << ", " << damage_sets[global::damage_index][0].data << std::endl;

#else
        std::seed_seq seed2(seed.begin(), seed.end());
        std::mt19937 gen(seed2);
        std::set<size_t> types = global::_pickSet(4, 2, gen); // two out of four types are selected randomly

        std::cout << "damage sets :" << std::endl;
        std::ofstream ofs("damage_sets_" + seed + ".txt");

        for (size_t el : types)
        {
            std::string damage_type = damage_types[el];
            for (size_t leg = 0; leg < 6; ++leg)
            {
                std::cout << damage_type << "," << leg << "\n";
                ofs << damage_type << "," << leg << "\n";
#ifndef TAKE_COMPLEMENT
                damage_sets.push_back({rhex_dart::RhexDamage(damage_type.c_str(), std::to_string(leg).c_str())});
#endif
            }
        }
        std::cout << std::endl;
#ifdef TAKE_COMPLEMENT
        types = global::_take_complement(global::_fullSet(4), types);
        ofs << "test:" << std::endl;
        for (size_t el : types)
        {
            std::string damage_type = damage_types[el];
            for (size_t leg = 0; leg < 6; ++leg)
            {
                std::cout << damage_type << "," << leg << "\n";
                ofs << damage_type << "," << leg << "\n";
                damage_sets.push_back({rhex_dart::RhexDamage(damage_type.c_str(), std::to_string(leg).c_str())});
            }
        }
        std::cout << std::endl;
#endif
#endif
        for (size_t i = 0; i < global::damage_sets.size(); ++i)
        {
            global::damaged_robots.push_back(std::make_shared<rhex_dart::Rhex>(robot_file, "Rhex", false, global::damage_sets[i])); // we repeat this creation process for damages
        }
    }
#endif

    void init_simu(std::string seed, std::string robot_file)
    {
        global::global_robot = std::make_shared<rhex_dart::Rhex>(robot_file, "Rhex", false, std::vector<rhex_dart::RhexDamage>()); // we repeat this creation process for damages
#if DAMAGE_TESTS()                                                                                                                 // damage tests (meta-learning with damages or test for damage recovery)
        init_damage(seed, robot_file);
#elif ENVIR_TESTS() // recovery tests (meta-learning with environments or test for environment adaptation)
        init_world(seed, robot_file);
#endif
    }
#if CMAES_CHECK()
    cmaes_t evo;
#endif
#if META()

    cmaes_t evo;
    // will use first-in-first-out queue such that latest DATABASE_SIZE individuals are maintained
    typedef SampledDataEntry data_entry_t;
    typedef CircularBuffer<BottomParams::MAX_DATABASE_SIZE, data_entry_t> database_t;
    database_t database;

#endif
} // namespace global

#endif
