
#ifndef CMAESCHECK_FITNESS_HPP
#define CMAESCHECK_FITNESS_HPP

#include <sferes/fit/fitness.hpp>
#include <meta-cmaes/cmaescheck_typedefs.hpp>
#ifdef TEST
#include <meta-cmaes/recovered_performance.hpp>
#endif
/* bottom-level fitmap 
used to evaluate behavioural descriptor and fitness of controllers in the normal operating environment
*/

namespace sferes
{
    namespace fit
    {

        SFERES_FITNESS(FitTaskMax, sferes::fit::Fitness)
        {

        public:
            FitTaskMax(){};
            inline void set_fitness(float fFitness)
            {
                this->_objs.resize(1);
                this->_objs[0] = fFitness;
                this->_value = fFitness;
            }

            template <typename Indiv>
            void eval(Indiv & indiv)
            {

                //std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

                this->_value = 0;
                _eval<Indiv>(indiv);
                //std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                // std::cout << "Time difference = " <<     std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
            }

        protected:
            // descriptor work done here, in this case duty cycle
            template <typename Indiv>
            void _eval(Indiv & indiv)
            {
#ifdef TEST
                float val = sferes::fit::RecoveredPerformance<Indiv>::_eval_single_envir(indiv, 0, global::damage_index);
#else
                float val = _eval_single_envir<Indiv>(indiv); 
#endif
                set_fitness(val);
            }
#ifndef TEST
            template <typename Phen>
            float _eval_single_envir(const Phen &indiv)
            {
                // copy of controller's parameters
                std::vector<double> _ctrl;
                _ctrl.clear();

                for (size_t i = 0; i < 24; i++)
                    _ctrl.push_back(indiv.gen().data(i));
#ifdef EVAL_ENVIR
                size_t world_option = global::envir_index;
		std::string dam = "none";
#else
                size_t world_option = 0;
                size_t damage_option = global::damage_index;
		std::string dam =  std::to_string(damage_option);
#endif

#ifdef GRAPHIC
                std::string fileprefix = "video" + std::to_string(world_option) + dam;
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

                simu.run(Params::simu::time); // run simulation for the same amount of time as the bottom level, to keep function evals comparable
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
#endif
        };
    } // namespace fit
} // namespace sferes

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// TOP   ////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// now that FitBottom is defined, define the rest of the bottom level
typedef sferes::fit::FitTaskMax<CMAESCHECKParams> fit_t;
typedef sferes::phen::Parameters<bottom_gen_t, fit_t, CMAESCHECKParams> phen_t;
typedef boost::shared_ptr<phen_t> bottom_indiv_t;

typedef sferes::eval::Eval<CMAESCHECKParams> eval_t;

#endif
