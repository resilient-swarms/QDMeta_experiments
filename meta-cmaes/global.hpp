
#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <rhex_dart/safety_measures.hpp>
#include <rhex_dart/descriptors.hpp>
#include <rhex_dart/rhex.hpp>
#include <meta-cmaes/feature_vector_typedefs.hpp>

#if META()
#include <meta-cmaes/params.hpp>
#include <sferes/ea/cmaes_interface.h>
#endif
namespace global
{
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
#if CMAES_CHECK()
size_t damage_index;
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
    std::ofstream ofs("world_options_" + seed + ".txt");
    std::seed_seq seed2(seed.begin(), seed.end());
    std::mt19937 gen(seed2);
    std::set<size_t> types = global::_pickSet(9, 5, gen);
    std::cout << "world options :" << std::endl;

    ofs << "{";
    for (size_t el : types)
    {
        size_t option = el + 1;// world 0 has been removed that is why +1
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
}
#endif

#if DAMAGE_TESTS()
std::vector<std::shared_ptr<rhex_dart::Rhex>> damaged_robots;
std::vector<std::vector<rhex_dart::RhexDamage>> damage_sets;
// {rhex_dart::RhexDamage("leg_removal", "26")}
void init_damage(std::string seed, std::string robot_file)
{
    std::vector<std::string> damage_types = {"leg_removal", "blocked_joint", "leg_shortening", "passive_joint"};
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
#if CMAES_CHECK()
	std::cout << "will do damage " << global::damage_index << ": " << damage_sets[global::damage_index][0].type << ", " << damage_sets[global::damage_index][0].data << std::endl;
#endif
#endif
    for (size_t i = 0; i < global::damage_sets.size(); ++i)
    {
        global::damaged_robots.push_back(std::make_shared<rhex_dart::Rhex>(robot_file, "Rhex", false, global::damage_sets[i])); // we repeat this creation process for damages
    }
}
#endif
#if GLOBAL_WEIGHT()
weight_t W;
void init_weight(std::string seed, std::string robot_file)
{
    std::ofstream ofs("global_weight_" + seed + ".txt");
    W = weight_t::Random();                //random numbers between (-1,1)
    W = (W + weight_t::Constant(1.)) / 2.; // add 1 to the matrix to have values between 0 and 2; divide by 2 --> [0,1]
    size_t count = 0;
#ifdef PRINTING
    std::cout << "before conversion " << std::endl;
#endif
    for (size_t j = 0; j < NUM_BOTTOM_FEATURES; ++j)
    {
        float sum = W.block<1, NUM_BASE_FEATURES>(j, 0).sum();
        for (size_t k = 0; k < NUM_BASE_FEATURES; ++k)
        {
            W(j, k) = W(j, k) / sum; // put it available for the MapElites parent class

#ifdef PRINTING
            std::cout << "sum " << sum << std::endl;
            std::cout << W(j, k) << "," << std::endl;
#endif
            ++count;
        }
    }
    ofs << W;
    std::cout << "global weight: " << std::endl;
    std::cout << W << std::endl;
}
#endif

void init_simu(std::string seed, std::string robot_file)
{
    global::global_robot = std::make_shared<rhex_dart::Rhex>(robot_file, "Rhex", false, std::vector<rhex_dart::RhexDamage>()); // we repeat this creation process for damages
#if DAMAGE_TESTS()                                                                                                             // damage tests (meta-learning with damages or test for damage recovery)
    init_damage(seed, robot_file);
#elif ENVIR_TESTS() // recovery tests (meta-learning with environments or test for environment adaptation)
    init_world(seed, robot_file);
#endif

#if GLOBAL_WEIGHT() // if using a global weight
    init_weight(seed, robot_file);
#endif
}

#if META()

cmaes_t evo;

struct SampledDataEntry
{
    std::vector<size_t> genotype;
    base_features_t base_features;
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    float fitness;
    SampledDataEntry() {}
    SampledDataEntry(const std::vector<size_t> &g, const base_features_t &b, const float &f) : genotype(g), base_features(b), fitness(f)
    {
    }
    template <typename Individual>
    void set_genotype(Individual &individual) const
    {
        for (size_t j = 0; j < individual->size(); ++j)
        {
            individual->gen().set_data(j, genotype[j]); // use the Sampled genotype API
        }
    }
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar &BOOST_SERIALIZATION_NVP(base_features);
        ar &BOOST_SERIALIZATION_NVP(fitness);
        ar &BOOST_SERIALIZATION_NVP(genotype);
    }
};

struct EvoFloatDataEntry
{
    std::vector<float> genotype;
    base_features_t base_features;
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    float fitness;
    EvoFloatDataEntry() {}
    EvoFloatDataEntry(const std::vector<float> &g, const base_features_t &b, const float &f) : genotype(g), base_features(b), fitness(f)
    {
    }
    // in case we want to use Evofloat instead
    template <typename Individual>
    void set_genotype(Individual &individual) const
    {
        for (size_t j = 0; j < individual->size(); ++j)
        {
            individual->gen().data(j, genotype[j]); // use the EvoFloat genotype API
        }
    }
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar &BOOST_SERIALIZATION_NVP(base_features);
        ar &BOOST_SERIALIZATION_NVP(fitness);
        ar &BOOST_SERIALIZATION_NVP(genotype);
    }
};

template <size_t capacity, typename DataType>
struct CircularBuffer
{
    size_t get_capacity()
    {
        return capacity;
    }
    CircularBuffer() : sp(0), max_sp(0)
    {
        data.resize(capacity);
    }
    std::vector<DataType> data;
    size_t sp;
    size_t max_sp;
    DataType &operator[](size_t idx)
    {
        return data[idx];
    }
    size_t size()
    {
        return max_sp;
    }
    void push_back(const DataType &d)
    {
        if (sp >= capacity)
        {
            // reset sp
            sp = 0;
        }
        data[sp] = d;
        if (max_sp < capacity)
            ++max_sp;
        ++sp;
    }

    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {

        ar &BOOST_SERIALIZATION_NVP(sp);
        ar &BOOST_SERIALIZATION_NVP(max_sp);
        ar &BOOST_SERIALIZATION_NVP(data);
    }
};

// struct DataBase   // filtering based on BD is problematic: either requires many checks or requires huge memory for fine-grained multi-array
// {
//   typedef std::array<typename array_t::index, behav_dim> behav_index_t;
//   typedef std::array<float, behav_dim> point_t;
//   typedef boost::multi_array<DataEntry, behav_dim> array_t;
//   behav_index_t behav_shape;
//   size_t nb_evals;
//   bottom_pop_t _pop;

//   weight_t W; //characteristic weight matrix of this map
//   // EIGEN_MAKE_ALIGNED_OPERATOR_NEW  // note not needed when we use NoAlign
//   DataBase()
//   {
//     assert(behav_dim == BottomParams::ea::behav_shape_size());
//     for (size_t i = 0; i < BottomParams::ea::behav_shape_size(); ++i)
//       behav_shape[i] = BottomParams::ea::behav_shape(i);
//     _array.resize(behav_shape);
//   }

//   void push_back(const base_features_t& b, const bottom_indiv_t& i1)
//   {
//     if (i1->fit().dead())
//       return;

//     point_t p = get_point(i1);
//     behav_index_t behav_pos;
//     for (size_t i = 0; i < BottomParams::ea::behav_shape_size(); ++i)
//     {
//       behav_pos[i] = round(p[i] * behav_shape[i]);
//       behav_pos[i] = std::min(behav_pos[i], behav_shape[i] - 1);
//       assert(behav_pos[i] < behav_shape[i]);
//     }

//     if (!_array(behav_pos) || (i1->fit().value() - _array(behav_pos)->fit().value()) > BottomParams::ea::epsilon || (fabs(i1->fit().value() - _array(behav_pos)->fit().value()) <= BottomParams::ea::epsilon && _dist_center(i1) < _dist_center(_array(behav_pos))))
//     {
//       _array(behav_pos) = i1;
//       return true;
//     }
//   }

//   point_t get_point()
//   {
//     point_t p;
//     for (size_t i = 0; i < NUM_BASE_FEATURES; ++i)
//       p[i] = std::min(1.0f, indiv->fit().desc()[i]);
//     return p;
//   }

// };

//typedef std::vector<DataEntry> database_t;// will become too long

// will use first-in-first-out queue such that latest DATABASE_SIZE individuals are maintained
typedef SampledDataEntry data_entry_t;
typedef CircularBuffer<BottomParams::MAX_DATABASE_SIZE, data_entry_t> database_t;
database_t database;

#endif
} // namespace global

#endif
