

#ifndef RASTRIGIN_FIT_HPP
#define RASTRIGIN_FIT_HPP
#include <math.h>
#include <meta-cmaes/feature_vector_typedefs.hpp>
#include <meta-cmaes/bottom_typedefs.hpp>
#include <Eigen/Dense>

#if CONTROL()
#include <modules/cvt_map_elites/fit_map.hpp>
#elif AURORA()
#include <sferes/fit/fit_qd.hpp>
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
#if NOT_AURORA()
            FitBottom() : _dead(false){};
#endif

#if META()
            feature_map_t feature_map;

            FitBottom(const feature_map_t &f) : feature_map(f), _dead(false)
            {
            }
#elif AURORA()
            FitBottom(){};
            float &entropy()
            {
                return _entropy;
            }
            const float &entropy() const { return _entropy; }
            const std::vector<float> &gt()
            {
                return _gt;
            }
            const std::vector<float> &successive_gt() const
            {
#warning "successive gt not defined; this is OK if not using LSTM"
                return _sgt;
            }
            template <typename block_t>
            void get_flat_observations(block_t & data) const
            {
                for (size_t t = 0; t < _gt.size(); t++)
                {
                    data(0, t) = _gt[t];
                }
            }

            size_t get_flat_obs_size() const
            {
                assert(_gt.size());
                return _gt.size();
            }

            float &implicit_fitness_value() { return this->_implicit_fitness_value; }
            const float &implicit_fitness_value() const { return this->_implicit_fitness_value; }

#endif
            inline void set_fitness(float fFitness)
            {
#if CONTROL() || AURORA()
                this->_objs.resize(1);
                this->_objs[0] = fFitness;
#endif
#if AURORA()
                this->_implicit_fitness_value = fFitness;
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

#if AURORA()
                ar &BOOST_SERIALIZATION_NVP(this->_dead);
                ar &BOOST_SERIALIZATION_NVP(this->_novelty);
                ar &BOOST_SERIALIZATION_NVP(this->_curiosity);
                ar &BOOST_SERIALIZATION_NVP(this->_lq);

                ar &BOOST_SERIALIZATION_NVP(this->_entropy);
                ar &BOOST_SERIALIZATION_NVP(this->_implicit_fitness_value);
                ar &BOOST_SERIALIZATION_NVP(this->_gt);
#endif
            }
#if NOT_AURORA()
            bool dead()
            {
                return _dead;
            }
#endif
            std::vector<float> ctrl()
            {
                return _ctrl;
            }
#if CONTROL()
            std::vector<float> get_descriptor(base_features_t & b)
            {
                return ctrl();
            }
#elif META()
            std::vector<float> get_descriptor(base_features_t & b)
            {
                //get the base_features
                get_base_features(b);
                set_b(b);
                // get descriptor
                return get_desc(b);
            }
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
                for (size_t i = 0; i < NUM_BASE_FEATURES; ++i)
                {
                    _b(i, 0) = -1.0f + 2.0f * features(i, 0);
                }
            }
            void set_b(const std::vector<float> &vec)
            {
                for (size_t i = 0; i < NUM_BASE_FEATURES; ++i)
                {
                    _b(i, 0) = -1.0f + 2.0f * vec[i];
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
            std::vector<float> _ctrl;
#if NOT_AURORA()
            bool _dead;
#endif
            base_features_t _b;

#if META() // these are already defined by FitMap
            mode::mode_t _mode;
            float _value = 0.0f;
            std::vector<float> _desc;
#elif AURORA()
            std::vector<float> _sgt;            // not used though
            std::vector<float> _gt;             // the 'ground truth' descriptor you would use for novelty search; for us we are interested in diversity over trajectories so same as base-BD
            float _entropy = -1;                //surprise-based selection and stats; value only set in the dimensionality_reduction.hpp modifier
            float _implicit_fitness_value = -1; // will be equal to the fitness for our case as we don't use pure divergent search
#endif
            static float evaluate_rastrigin(std::vector<float> & xx)
            {
                float sum = 10.0f * RASTRI_DIM;
                float A = 10.0f;
                std::vector<float> x = xx;
                for (size_t i = 0; i < RASTRI_DIM; ++i)
                {
                    x[i] = RASTRI_MIN + RASTRI_RANGE * xx[i];
#ifdef GRAPHIC
                    std::cout << x[i] << " ";
#endif
                    sum += (x[i] * x[i] - A * std::cos(2 * M_PI * x[i]));
                }
#ifdef GRAPHIC
                std::cout << "f(x)=" << -sum << std::endl;
#endif
                return -sum; //maximisation rather than minimisation
            }
            // descriptor work done here, in this case duty cycle
            template <typename Indiv>
            void _eval(Indiv & indiv)
            {

                // input parameters
                _ctrl.clear();

                for (size_t i = 0; i < RASTRI_DIM; i++)
                    _ctrl.push_back(indiv.data(i)); // [0,1] -> [-5.12,5.12]

                set_fitness(evaluate_rastrigin(_ctrl));

                std::vector<float> desc;

#if AURORA()
#ifdef PRINTING
                std::cout << " get desc " << std::endl;
#endif
                get_base_features();

#ifdef PRINTING
                std::cout << " end get desc " << std::endl;
#endif
#else
                // convert to final descriptor
                base_features_t b;
                this->_desc = get_descriptor(b);
#endif
                this->_dead = false;

#ifdef GRAPHIC
                std::cout << " base-features " << _b.transpose() << std::endl;
                std::cout << " fitness is " << this->_value << std::endl;
#endif
            }

#if AURORA()
            /* the included descriptors determine the base-features */
            void get_base_features()
            {
                this->_gt.clear();
                this->_gt = _ctrl;
            }

#else
            /* the included descriptors determine the base-features */
            void get_base_features(base_features_t & base_features)
            {

                for (size_t i = 0; i < _ctrl.size(); ++i)
                {
                    base_features(i, 0) = _ctrl[i];
                }
#endif
        }
    };
} // namespace fit
} // namespace sferes

#endif
