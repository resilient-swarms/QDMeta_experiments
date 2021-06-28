#include <iostream>
#include <cstdlib>
#include <rhex_dart/rhex_dart_simu.hpp>

//std::vector<std::string> damage_types = {"leg_removal", "blocked_joint", "leg_shortening", "passive_joint"};

int main(int argc, char **argv)
{
    // using the same model as the hexapod and so the robot has a damages parameter but is set to 0
    std::vector<rhex_dart::RhexDamage> damages(0);
    size_t world_option = 0;
    int ctrl_size = 24;  
    // loads the robot with name RHex, not a URDF file, and blank damage
    std::string model_file = std::string(std::getenv("BOTS_DIR")) + "/share/rhex_models/SKEL/raised.skel";
    std::string robot_name = "Rhex";
    auto global_robot = std::make_shared<rhex_dart::Rhex>(model_file, robot_name, false, damages); // we repeat this creation process for damages

    //note: boost fusion vector at most 10 template arguments
    using desc_t = boost::fusion::vector<rhex_dart::descriptors::DeltaFullTrajectory>;

    using safe_t = boost::fusion::vector<rhex_dart::safety_measures::TurnOver>;
    using viz_t = boost::fusion::vector<rhex_dart::visualizations::HeadingArrow, rhex_dart::visualizations::RobotTrajectory>;
    using simu_t = rhex_dart::RhexDARTSimu<rhex_dart::desc<desc_t>, rhex_dart::safety<safe_t>, rhex_dart::viz<viz_t>>;
    // const std::vector<double> &ctrl,
    // robot_t robot
    // int world_option = 0
    // double friction = 1.0
    // std::vector<rhex_dart::RhexDamage> damages = {}
    size_t required_solutions = 10000;
    std::ofstream ofs("centroid_trajectories"+std::string(argv[1])+".txt");
    size_t legal_solutions = 0;
    std::mt19937 gen(std::atoi(argv[1]));
    std::uniform_real_distribution<double> dis(0.0, 1.0);
    while (legal_solutions < required_solutions)
    {

        std::vector<double> ctrl;
        for (int j = 1; j <= ctrl_size; ++j)
        {
            double r = dis(gen);
            ctrl.push_back(r);
            //std::cout << r << " ";
        }
        std::cout << std::endl;
        simu_t simu(ctrl, global_robot->clone(), world_option, 1.0, damages);
        double time = 5.0;
        simu.run(time);
        std::vector<double> v;
        simu.get_descriptor<rhex_dart::descriptors::DeltaFullTrajectory>(v);
        float dead = -1000.0f;

        if (dead > simu.covered_distance())
        {
            //std::cout << "dead" << std::endl;
            continue;
        }
        else
        {
            for (size_t i = 0; i < v.size(); ++i)
            {
                //std::cout << v[i] << " ";
                ofs << v[i] << " ";
            }
            //std::cout << std::endl;
            ofs << std::endl;
            legal_solutions += 1;
        }
    }
    ofs.close();

    return 0;
}
