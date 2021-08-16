#ifndef EVAL_META_HPP
#define EVAL_META_HPP

#include <cmath>
#include <sferes/eval/eval.hpp>
#include <vector>
#include <meta-cmaes/feature_vector_typedefs.hpp>
#include <meta-cmaes/bottom_typedefs.hpp>
#include <meta-cmaes/eval_parallel.hpp>

namespace sferes
{
      namespace eval
      {
            template <typename Phen, typename Fit>
            struct _eval_serial_meta
            {
                  size_t nb_evals;
                  float value;
                  _eval_serial_meta(Phen &meta_i, float percent)
                  {
                        std::vector<boost::shared_ptr<base_phen_t>> pop = meta_i.sample_individuals(percent);
                        value = 0.0f;
#if DIMENSION_TESTS()
                        float spwd = 0; //summed pairwise distance
                        //maximal total value is RASTRI_DIM/2*pop.size()*MAXFIT, so keep spwd on same scale
                        //spwd in [0,\sqrt(RASTRI_DIM)*pop.size() * (pop.size() - 1)/2] --> [0,RASTRI_DIM/2*pop.size()*MAXFIT ]
                        //requires the scaling factor sqrt(RASTRI_DIM)*MAXFIT/(pop.size() - 1)
                        float spwd_scale = std::sqrt(RASTRI_DIM) * MAXFIT / (pop.size() - 1);
#endif
                        for (size_t i = 0; i < pop.size(); ++i)
                        {
                              // evaluate the individual
                              value += Fit::_eval_all(*pop[i]);
#if DIMENSION_TESTS()

                              base_features_t pos = 0.5* pop[i]->fit().b();//[-1,1] -> [-0.5,0.5]
                              for (size_t j = i + 1; j < pop.size(); ++j)
                              {
                                    base_features_t pos2 = 0.5*pop[j]->fit().b();
#ifdef GRAPHIC
                                    std::cout << "pos " << pos.transpose() << std::endl;
                                    std::cout << "pos2 " << pos2.transpose() << std::endl;
#endif
                                    spwd += (pos - pos2).norm(); // [0,\sqrt(RASTRI_DIM)]  
                              }

#endif
                        }
#ifdef GRAPHIC
                        std::cout << "value  " << value << std::endl;
#endif 

#if DIMENSION_TESTS()
                        float scaled_spwd = spwd * spwd_scale;
                        value+=scaled_spwd;
#ifdef GRAPHIC
                        std::cout << "spwd " << spwd << std::endl;
                        std::cout << "scaled spwd " << scaled_spwd << std::endl;
                        std::cout << "value + scaled_pwd " << value << std::endl;
#endif
                        
#endif
                        std::tuple<float, int> results = meta_i.fit().avg_value(value);
                        value = std::get<0>(results);
                        nb_evals = std::get<1>(results);
                  }
            };

            template <typename Phen, typename Fit>
            struct _eval_parallel_meta : public _eval_parallel_individuals<CSharedMem, base_phen_t, bottom_fit_t>
            {
                  float value;
                  size_t nb_evals;
                  Phen meta_indiv;
                  _eval_parallel_meta(Phen &meta_i, float percent)
                  { //now join the bottom-level fitnesses
                        this->_pop = meta_i.sample_individuals(percent);
                        this->run();
                  }

                  virtual void LaunchSlave(size_t slave_id)
                  {
                        // we sample directly from map of developed individuals, so the individuals do not need to be developed, nor does their fitness prototype be set
                        float fitness = Fit::_eval_all(*this->_pop[slave_id]);
                        // write fitness and death value (no descriptors) to shared memory
                        shared_memory[slave_id]->setFitness(fitness); // ASSUME SINGLE OBJECTIVE
                                                                      //shared_memory[slave_id]->setDeath(false);// no need
#ifdef CHECK_PARALLEL
                        std::cout << "child fitness " << slave_id << " " << fitness << std::endl;
#endif
                        this->quit();
                  }

                  virtual void write_data()
                  {
                        value = 0.0;
                        /* Back in the parent, copy the scores into the population data */
                        for (size_t i = 0; i < this->_pop.size(); ++i)
                        {
                              float temp = shared_memory[i]->getFitness();
                              value += temp;
#ifdef CHECK_PARALLEL
                              std::cout << "parent fitness " << i << " " << temp << std::endl;
#endif
                        }
                        std::tuple<float, size_t> results = meta_indiv.fit().avg_value(value);
                        value = std::get<0>(results);
                        nb_evals = std::get<1>(results);
                  }
            };

      } // namespace eval
} // namespace sferes

#endif