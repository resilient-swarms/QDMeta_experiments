

#ifndef FITBOTTOM_HPP
#define FITBOTTOM_HPP

#include <meta-cmaes/feature_vector_typedefs.hpp>
#include <meta-cmaes/bottom_typedefs.hpp>
#include <Eigen/Dense>

#if CONTROL()
#include <modules/cvt_map_elites/fit_map.hpp>
#elif AURORA()
#include <aurora/fit/fit_qd.hpp>
#endif

/* bottom-level fitmap 
used to evaluate behavioural descriptor and fitness of controllers in the normal operating environment
*/

namespace sferes
{
    namespace fit
    {
#if CONTROL() 

        SFERES_FITNESS(FitBottom, sferes::fit::FitMap)
        {
#elif AURORA()
       FIT_QD(FitBottom)
	{ 
#else
        class FitBottom
        {
#endif

public:
    FitBottom() : _dead(false){};
#if META()
    feature_map_t feature_map;

    FitBottom(const feature_map_t &f) : feature_map(f), _dead(false)
    {
    }
#elif AURORA()
	   const std::vector<float>& gt() {return _gt;}
  	   float& loss() {return _loss;}
  template<typename block_t>
  void get_flat_observations(block_t& data)const
  {
    for(size_t t=0; t< _gt.size(); t++)
      {
        data(0,t) = _gt[t];
      }
  }

  size_t get_flat_obs_size()const{
    assert(_gt.size());
    return _gt.size();
  }

#endif
    inline void set_fitness(float fFitness)
    {
#if CONTROL()
        this->_objs.resize(1);
        this->_objs[0] = fFitness;
#endif
        this->_value = fFitness;
    }
    inline void set_dead(bool dead)
    {
        this->_dead = dead;
    }

    template <typename Indiv>
    void eval(Indiv & indiv)
    {

        //std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

        this->_value = 0;
        this->_dead = false;
        _eval<Indiv>(indiv);
        //std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        // std::cout << "Time difference = " <<     std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
    }

    // override the function so that we can write behaviour descriptor values along with the fitness value
    template <class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar &BOOST_SERIALIZATION_NVP(this->_value);
        ar &BOOST_SERIALIZATION_NVP(this->_desc);
    }

    bool dead() { return _dead; }
    std::vector<double> ctrl() { return _ctrl; }

    std::vector<float> get_desc(simulator_t & simu, base_features_t & b)
    {
#if !META()
                std::vector<double> vec;
                if (global::condition == global::ConditionType::tra)
                {
                    //std::cout << "pol "<<std::endl;
                    simu.get_descriptor<rhex_dart::descriptors::Trajectory, std::vector<double>>(vec);
                }
                else
                {
                    throw std::runtime_error("Please give a viable condition");
                }
                return std::vector<float>(vec.begin(), vec.end());
#else
                //get the base_features
                get_base_features(b, simu);
                // get descriptor
                return get_desc(b);
#endif  // NO_WEIGHT() else WEIGHT()
    }
#if META()
            std::vector<float> get_desc(const base_features_t &b)
            {

		//std::cout << "META "<<std::endl;
                bottom_features_t D = feature_map.out(b);
                std::vector<float> vec(D.data(), D.data() + D.rows() * D.cols());
                return vec;
            }
    template <typename Individual>
    static void add_to_database(Individual & ind)
    {
        if (!ind.fit().dead())
        {
            //push to the database
            global::database.push_back(global::data_entry_t(ind.gen().data(), ind.fit().b(), ind.fit().value()));
#ifdef PRINTING
            std::cout << " adding entry with fitness " << ind.fit().value() << std::endl;
#endif
        }
    }
    base_features_t b()
    {
        return _b;
    }
    void set_b(const base_features_t &features)
    {
        _b = features;
    }
    void set_b(const std::vector<float> &vec)
    {
        for (size_t i = 0; i < NUM_BASE_FEATURES; ++i)
        {
            _b(i, 0) = vec[i];
        }
    }
    mode::mode_t mode() const
    {
        return _mode;
    }
    void set_mode(mode::mode_t m)
    {
        _mode = m;
    }

    void set_desc(const std::vector<float> &d)
    {
        _desc = d;
    }

    void set_value(float v)
    {
        _value = v;
    }
    std::vector<float> desc()
    {
        return _desc;
    }

    float value()
    {
        return _value;
    }
#endif

protected:
    std::vector<double> _ctrl;
    bool _dead;
    base_features_t _b;

#if META() // these are already defined by FitMap
    mode::mode_t _mode;
    float _value = 0.0f;
    std::vector<float> _desc;
#elif AURORA()
	    std::vector<float> _gt;
	    float _loss=INFINITY;
#endif



    // descriptor work done here, in this case duty cycle
    template <typename Indiv>
    void _eval(Indiv & indiv)
    {

        // copy of controller's parameters
        _ctrl.clear();

        for (size_t i = 0; i < 24; i++)
            _ctrl.push_back(indiv.data(i));

        // launching the simulation
        auto robot = global::global_robot->clone();

        simulator_t simu(_ctrl, robot);
        simu.run(BottomParams::simu::time); // run simulation for 5 seconds

        set_fitness(simu.covered_distance());

        std::vector<float> desc;

        float dead = -1000.0f;
        // these assume a behaviour descriptor of size 6.
        if (dead > this->_value)
        {
            // this means that something bad happened in the simulation
            // we kill this individual
            this->_dead = true; // no need to do anything
                                // desc.resize(6);
                                // desc[0] = 0;
                                // desc[1] = 0;
                                // desc[2] = 0;
                                // desc[3] = 0;
                                // desc[4] = 0;
                                // desc[5] = 0;
                                // this->_value = -1000.0f;// note this causes troubles;
                                // -> due to optimisation (presumably) the code is evaluated within if first, therefore the above condition seems to always be true
        }
        else
        {
            // convert to final descriptor
            base_features_t b;
#if AURORA()
#ifdef PRINTING
		    std::cout << " get desc " << std::endl;
#endif
		    this->_gt = get_desc(simu, b);
#ifdef PRINTING
		    std::cout << " end get desc " << std::endl;
#endif
#else
            this->_desc = get_desc(simu, b);
#endif
            this->_dead = false;
#if META()
            set_b(b);
#endif
#ifdef PRINTING
            std::cout << " fitness is " << this->_value << std::endl;
#endif

        }
    }

    /* the included descriptors determine the base-features */
    void get_base_features(base_features_t & base_features, simulator_t & simu)
    {

        std::vector<double> results;
        simu.get_descriptor<rhex_dart::descriptors::Trajectory, std::vector<double>>(results);

        for (size_t i = 0; i < results.size(); ++i)
        {
            base_features(i, 0) = results[i];
        }
    }
};
} // namespace fit
} // namespace sferes

#endif
