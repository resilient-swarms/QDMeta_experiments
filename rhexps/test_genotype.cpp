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
    if (argc == ctrl_size + 3)
    {
        // damage_type, leg affected are the two arguments
        damages.push_back(rhex_dart::RhexDamage(argv[ctrl_size + 1], argv[ctrl_size + 2]));
    }
    else if (argc == ctrl_size + 2)
    {
        // world option as one additional argument
        world_option = std::stoi(argv[ctrl_size + 1]);
    }
    else
    {
        assert(argc == ctrl_size + 1);
    }

    srand(time(NULL)); //set seed

    // loads the robot with name RHex, not a URDF file, and blank damage
    std::string model_file = std::string(std::getenv("BOTS_DIR")) + "/share/rhex_models/SKEL/raised.skel";
    std::string robot_name = "Rhex";
    auto global_robot = std::make_shared<rhex_dart::Rhex>(model_file, robot_name, false, damages); // we repeat this creation process for damages

    std::vector<double> ctrl;
    for (int j = 1; j <= ctrl_size; ++j)
    {
        ctrl.push_back(atof(argv[j]));
    }

    //note: boost fusion vector at most 10 template arguments
    using desc_t = boost::fusion::vector<rhex_dart::descriptors::FullTrajectory>;

    using safe_t = boost::fusion::vector<rhex_dart::safety_measures::TurnOver>;
    using viz_t = boost::fusion::vector<rhex_dart::visualizations::HeadingArrow, rhex_dart::visualizations::RobotTrajectory>;
    using simu_t = rhex_dart::RhexDARTSimu<rhex_dart::desc<desc_t>, rhex_dart::safety<safe_t>, rhex_dart::viz<viz_t>>;
    // const std::vector<double> &ctrl,
    // robot_t robot
    // int world_option = 0
    // double friction = 1.0
    // std::vector<rhex_dart::RhexDamage> damages = {}
    simu_t simu(ctrl, global_robot->clone(), world_option, 1.0, damages);
    double time = 5.0;
    simu.run(time);
    std::vector<double> v;
    simu.get_descriptor<rhex_dart::descriptors::FullTrajectory>(v);
    std::cout << "TRA:" << std::endl;
    for (size_t i = 0; i < v.size(); i++)
    {
        std::cout << v[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "fitness " << simu.covered_distance() << std::endl;

    return 0;
}
