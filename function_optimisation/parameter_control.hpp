
#ifndef PARAMETER_CONTROL_HPP
#define PARAMETER_CONTROL_HPP

#include <string>
#include <meta-cmaes/global.hpp>
#include <meta-cmaes/params.hpp>
#include <meta-cmaes/parameter_controller.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
struct EvalStats
{
    float best_metafitness;
    float sd_metafitness;
    float avg_metafitness;
    float metagenotype_diversity;
    template <typename MetaIndiv>
    void get_diversity(std::vector<boost::shared_ptr<MetaIndiv>> &pop)
    {
        metagenotype_diversity = 0.0f;
        for (size_t i = 0; i < pop.size(); ++i)
        {
            for (size_t j = i + 1; j < pop.size(); ++j)
            {
                metagenotype_diversity += pop[i]->dist(pop[j]);
            }
        }
    }
    template <typename MetaIndiv>
    void set_stats(std::vector<boost::shared_ptr<MetaIndiv>> &pop)
    {
        best_metafitness = -INFINITY;
        sd_metafitness = 0;
        avg_metafitness = 0;
        for (size_t i = 0; i < pop.size(); ++i)
        {
            float newfit = pop[i]->fit().value();
            if (newfit > best_metafitness)
            {
                best_metafitness = newfit;
            }
            avg_metafitness += newfit;
        }
        avg_metafitness /= (float)pop.size();
        for (size_t i = 0; i < pop.size(); ++i)
        {
            float deviation = (pop[i]->fit().value() - avg_metafitness);
            sd_metafitness += (deviation * deviation);
        }
        sd_metafitness = std::sqrt(sd_metafitness / (float)pop.size());
        get_diversity<MetaIndiv>(pop);
    }

} eval_stats;

std::string mutationlogfile, epochslogfile, rewardlogfile;

void write_mutation(float mutation_rate)
{
    const char *file = mutationlogfile.c_str();
    std::ofstream mutationlog(file, std::ios::app);
    mutationlog << mutation_rate << " " << global::nb_evals << std::endl;
    mutationlog.close();
}

void write_epochs(int bot_epochs)
{
    const char *file = epochslogfile.c_str();
    std::ofstream epochslog(file, std::ios::app);
    epochslog << bot_epochs << " " << global::nb_evals << std::endl;
    epochslog.close();
}

void write_reward(double reward, double meta_fitness)
{
    const char *file = rewardlogfile.c_str();
    std::ofstream rewardlog(file, std::ios::app);
    rewardlog << reward << " " << meta_fitness << " " << global::nb_evals << std::endl;
    rewardlog.close();
}

//bottom params
struct ParameterControl
{
    float bottom_epochs_factor = 1.0f;
    float percentage_evaluated_factor = 1.0f;
    float mutation_rate_factor = 1.0f;
    phen_t phenotype;
    EvalStats eval_stats;
    size_t evaluations = 0;
    virtual ~ParameterControl(){};
    ParameterControl()
    {
    }
    ParameterControl(float bf, float pf, float mf = 1.0f)
    {
        this->bottom_epochs_factor = bf;
        this->percentage_evaluated_factor = pf;
        this->mutation_rate_factor = mf;
    }

    /* update the parameter control algorithm */
    virtual void update()
    {
    }
    /* get the number of bottom epochs per meta-individual */
    virtual int get_bottom_epochs()
    {
        int bot_epochs = (int)std::round(this->bottom_epochs_factor * BottomParams::bottom_epochs);
        //std::cout << "bottom epochs " << bot_epochs << std::endl;
        write_epochs(bot_epochs);
        return bot_epochs;
    }
    /* get the percentage of solutions to be evaluated for meta-fitness computation */
    virtual float get_percentage_evaluated()
    {
        float percentage = this->percentage_evaluated_factor * CMAESParams::percentage_evaluated;
        //std::cout << "percentage = " << percentage << std::endl;
        return percentage;
    }

    virtual void set_stats(EvalStats &eval_stats)
    {
        this->eval_stats = eval_stats;
    }

    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        // reset the data-base
        //ar &BOOST_SERIALIZATION_NVP(eval_stats);// don't care as these stats are only used one time (before saving)
        // phenotype similarly
        ar &BOOST_SERIALIZATION_NVP(bottom_epochs_factor);
        ar &BOOST_SERIALIZATION_NVP(percentage_evaluated_factor);
        ar &BOOST_SERIALIZATION_NVP(mutation_rate_factor);
        ar &BOOST_SERIALIZATION_NVP(evaluations);
    }
};

//bottom params
struct EpochAnnealing : public ParameterControl
{
    friend class boost::serialization::access;
    float min_bottom_epochs = 1.0f;
    EpochAnnealing()
    {
    }
    EpochAnnealing(float bf, float pf) : ParameterControl(bf, pf)
    {
    }
    virtual ~EpochAnnealing(){};
    virtual int get_bottom_epochs()
    {
        float ratio = (float)(CMAESParams::pop::max_evals - global::nb_evals) / (float)CMAESParams::pop::max_evals;
        int bot_epochs = (int)std::round(this->min_bottom_epochs + ratio * (this->bottom_epochs_factor * BottomParams::bottom_epochs - this->min_bottom_epochs)); //
        write_epochs(bot_epochs);
        return bot_epochs;
    }

    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        // serialize base class information
        ar &BOOST_SERIALIZATION_BASE_OBJECT_NVP(ParameterControl);

        ar &BOOST_SERIALIZATION_NVP(min_bottom_epochs);
    }
};

BOOST_CLASS_EXPORT_KEY(EpochAnnealing)
BOOST_SERIALIZATION_SHARED_PTR(EpochAnnealing)

//bottom params
struct EpochEndogenous : public ParameterControl
{
    friend class boost::serialization::access;
    float min_bottom_epochs = 1.0f;
    EpochEndogenous()
    {
    }
    EpochEndogenous(float bf, float pf) : ParameterControl(bf, pf)
    {
    }
    virtual ~EpochEndogenous(){};
    virtual int get_bottom_epochs()
    {
        size_t last = NUM_GENES - 1;
        float last_gene = this->phenotype.gen().data()[last];
        float M = CMAESParams::parameters::max;
        float m = CMAESParams::parameters::min;
        last_gene = (last_gene - m) / (M - m); // normalise in [0,1] 
        int bot_epochs = (int)std::round(this->min_bottom_epochs + last_gene * (this->bottom_epochs_factor * BottomParams::bottom_epochs - this->min_bottom_epochs)); //
        write_epochs(bot_epochs);
        return bot_epochs;
    }

    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        // serialize base class information
        ar &BOOST_SERIALIZATION_BASE_OBJECT_NVP(ParameterControl);

        ar &BOOST_SERIALIZATION_NVP(min_bottom_epochs);
    }
};

BOOST_CLASS_EXPORT_KEY(EpochEndogenous)
BOOST_SERIALIZATION_SHARED_PTR(EpochEndogenous)



//bottom params
struct RL : public ParameterControl
{
    friend class boost::serialization::access;
    float f_last;
    size_t stagnation = 0;
    size_t num_params;
    RLController controller;
    float mutation_rate, bottom_epochs;
    float scale;                    //scale accounting for average number of bottom-level evaluations
    const float reward_max = 10.0f; // beyond 10-fold improvement is not meaningful
    RL() {}
    RL(long seed, std::string &parameter, float bf, float pf, float mf) : ParameterControl(bf, pf, mf)
    {
        scale = CMAESParams::pop::size * (BottomParams::bottom_epochs * 2 * BottomParams::pop::size + CMAESParams::percentage_evaluated * 10000.0f);
        controller = RLController();
        if (parameter == "bottom_epochs")
        {
            controller.addParameter("bottom_epochs", ParameterController::ParameterType::NUMERIC_INT_, 1.f, this->bottom_epochs_factor * BottomParams::bottom_epochs);
            num_params = 1;
        }
        else
        {
            throw std::runtime_error("not implemented");
        }
        controller.initialize(seed, "bins:5");
    }
    virtual ~RL(){};

    virtual int get_bottom_epochs()
    {
        int be = (int)std::round(bottom_epochs);
        write_epochs(be);
        return be;
    }

    virtual void set_stats(EvalStats &eval_stats)
    {
        this->eval_stats = eval_stats;
        this->eval_stats.best_metafitness = this->eval_stats.best_metafitness;
        float cf = this->eval_stats.best_metafitness;
        float ratio = 0;
        float rwrd;
        if (f_last == 0 && cf > 0)  
        {
            rwrd = reward_max;
        }
        else if (cf >= 0 && f_last >= 0)
        {
            ratio = cf / f_last;
            rwrd = std::min(reward_max, scale * (ratio - 1) / (global::nb_evals - this->evaluations));
        }
        else{
            throw std::runtime_error("meta-fitness cannot be negative; if this is not an error, please reformulate the meta-fitness");
        }
        
        if (rwrd < 0) 
            rwrd = 0;

        
        write_reward(rwrd,cf);
        std::vector<float> obs(7);
        obs[0] = rwrd;
        obs[1] = this->eval_stats.best_metafitness;
        obs[2] = this->eval_stats.avg_metafitness;
        obs[3] = this->eval_stats.sd_metafitness;
        obs[4] = this->eval_stats.metagenotype_diversity;
        obs[5] = stagnation;
        obs[6] = obs[0];
        std::cout << "obs " << std::endl;
        for (size_t i = 0; i < obs.size(); ++i)
        {
            std::cout << obs[i] << ", ";
        }
        std::cout << std::endl;
        controller.updateObservables(obs);
        // Update parameters
        float normal_be = ParameterControl::get_bottom_epochs();
        bottom_epochs = controller.getNextValue("bottom_epochs", normal_be);

        // Update stagnation count
        if (cf > f_last)
        {
            // An improvement was made, reset the stagnation count
            stagnation = 0;
        }
        else
        {
            stagnation++;
        }
        // Update the last current best fitness
        f_last = this->eval_stats.best_metafitness;
        // Update evaluations
        this->evaluations = global::nb_evals;
    }

    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        // serialize base class information
        ar &BOOST_SERIALIZATION_BASE_OBJECT_NVP(ParameterControl);

        ar &BOOST_SERIALIZATION_NVP(stagnation);
        ar &BOOST_SERIALIZATION_NVP(num_params);
        ar &BOOST_SERIALIZATION_NVP(controller);
        ar &BOOST_SERIALIZATION_NVP(bottom_epochs);
        ar &BOOST_SERIALIZATION_NVP(scale); //scale accounting for average number of bottom-level evaluations
        //ar &BOOST_SERIALIZATION_NVP(reward_max); // beyond 10-fold improvement is not meaningful
    }
};
BOOST_CLASS_EXPORT_KEY(RL)
BOOST_SERIALIZATION_SHARED_PTR(RL)


namespace sferes
{
    namespace eval
    {
        boost::shared_ptr<ParameterControl> param_ctrl;
    }
} // namespace sferes
#endif