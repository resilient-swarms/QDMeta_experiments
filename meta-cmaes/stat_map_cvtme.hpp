#ifndef CVT_MAP_ELITES_STAT_MAP_HPP_
#define CVT_MAP_ELITES_STAT_MAP_HPP_

#include <boost/multi_array.hpp>
#include <limits>
#include <numeric>
#include <sferes/stat/stat.hpp>
#include <meta-cmaes/recovered_performance.hpp>

#define MAP_WRITE_PARENTS // no longer used in cvt-map-elites -- was functional in map-elites

namespace sferes
{
    namespace stat
    {
        namespace _stat_map
        {
            // this is a nice (complicated) trick to detect if the type T has a member named 'size'
            // we need this because we might be unable to print the data (e.g. for a neural network)
            template <typename T, typename = int>
            struct HasSize : std::false_type
            {
            };
            template <typename T>
            struct HasSize<T, decltype(&T::size, 0)> : std::true_type
            {
            };

            class DataPrinter
            {
            public:
                template <class T>
                typename std::enable_if<HasSize<T>::value, void>::type print(const T &gen, std::ofstream &ofs) const
                {
                    for (size_t k = 0; k < gen.size(); ++k)
                        ofs << gen.data(k) << " ";
                }
                template <class T>
                typename std::enable_if<!HasSize<T>::value, void>::type print(const T &gen, std::ofstream &ofs) const
                {
                    // do nothing
                }
            };
        } // namespace _stat_map

        SFERES_STAT(Map, Stat)
        {
        public:
            typedef boost::shared_ptr<Phen> bottom_indiv_t;
            typedef boost::array<float, Params::ea::number_of_dimensions> point_t;

            size_t number_of_dimensions;

            Map() : number_of_dimensions(Params::ea::number_of_dimensions) {}

            template <typename E>
            void refresh(const E &ea)
            {
                _archive.clear();
                for (size_t i = 0; i < ea.archive().size(); ++i)
                    _archive.push_back(ea.archive()[i]);

                //                Progress is already being written in state_progress, so why write it again here
                //                this->_create_log_file(ea, "progress_archive.dat");
                //                _write_progress(ea, *this->_log_file);

                if (ea.gen() % Params::pop::dump_period == 0)
                {
                    _write_archive(ea.archive(), std::string("archive_"), ea);
                }
            }

            void show(std::ostream & os, size_t n)
            {
                std::cout << "NUM_CORES " << NUM_CORES << std::endl;
                std::cout << "show stat" << std::endl;
                std::cout << "read the archive" << std::endl;

                std::vector<bottom_indiv_t> individuals;
                for (bottom_indiv_t *k = _archive.data(); k < (_archive.data() + _archive.size()); ++k)
                {
                    if (*k)
                    {
                        individuals.push_back(*k);
                    }
                }
#ifdef GRAPHIC // we are just interested in observing a particular individual
                float val = 0.0f;
                size_t count = 0;
                std::cout << "loading individual" << n << std::endl;
                for (bottom_indiv_t indiv : individuals)
                {
                    if (count == n)
                    {
                        float val = sferes::fit::RecoveredPerformance<Phen>::_eval_all(*indiv);
                        std::cout << val << std::endl;
                        break;
                    }
                    ++count;

                    //std::cout << count;
                }

#else
#ifdef INDIVIDUAL_DAMAGE
                sferes::fit::RecoveredPerformance<Phen>::test_max_recovery(os, individuals);
#else
                sferes::fit::RecoveredPerformance<Phen>::test_recoveredperformance(os, individuals);
#endif
#endif
            }

            template <class Archive>
            void serialize(Archive & ar, const unsigned int version)
            {
                ar &BOOST_SERIALIZATION_NVP(_archive);
                ar &BOOST_SERIALIZATION_NVP(number_of_dimensions);
            }

        protected:
            std::vector<bottom_indiv_t> _archive;

            template <typename EA>
            void _write_archive(const std::vector<bottom_indiv_t> &archive,
                                const std::string &prefix, const EA &ea) const
            {
                std::cout << "writing..." << prefix << ea.gen() << std::endl;
                std::string fname = ea.res_dir() + "/" + prefix +
                                    boost::lexical_cast<std::string>(ea.gen()) +
                                    std::string(".dat");

                std::ofstream ofs(fname.c_str());

                for (size_t i = 0; i < archive.size(); ++i)
                {
                    if (archive[i])
                    {
                        // Write the index of the individual
                        ofs << i << "  ";
                        // Write the descriptor
                        std::vector<float> desc = archive[i]->fit().desc();
                        for (size_t j = 0; j < number_of_dimensions; ++j)
                            ofs << std::setprecision(3) << desc[j] << "  ";

                        // Write the fitness
                        ofs << " " << archive[i]->fit().value() << "  ";

                        // this will print only if there is a size() member in the genotype
                        // (which means that we have some kind of vector)
                        sferes::stat::_stat_map::DataPrinter().print(archive[i]->gen(), ofs);
                        ofs << std::endl;
                    }
                }
            }

            //            Progress is already being written in state_progress, so why write it again here
            //            template <typename EA>
            //            void _write_progress(const EA &ea, std::ofstream &ofs) const
            //            {
            //                std::cerr << "_write_progress " << std::endl;

            //                double archive_min = std::numeric_limits<double>::max();
            //                double archive_max = std::numeric_limits<double>::lowest();
            //                double archive_mean = 0.0;
            //                size_t archive_size = 0;

            //                std::vector<phen_t> archive = ea.archive();

            //                for (size_t i = 0; i < archive.size(); ++i)
            //                {
            //                    if (archive[i])
            //                    {
            //                        archive_size++;

            //                        archive_mean += archive[i]->fit().value();

            //                        if (archive[i]->fit().value() < archive_min)
            //                            archive_min = archive[i]->fit().value();

            //                        if (archive[i]->fit().value() > archive_max)
            //                            archive_max = archive[i]->fit().value();
            //                    }
            //                }

            //                archive_mean /= archive.size();

            //                ofs << ea.gen() << " " << ea.nb_evals() << " " << archive_size << " "
            //                    << archive_min << " " << archive_mean << " " << archive_max
            //                    << std::endl;
            //            }
        };
    } // namespace stat
} // namespace sferes

#endif