#ifndef RHEX_DART_DESCRIPTORS_HPP
#define RHEX_DART_DESCRIPTORS_HPP

#include <algorithm>
#include <map>
#include <vector>
#include <numeric>

#include <Eigen/Core>

#include <rhex_dart/rhex.hpp>

namespace rhex_dart
{

    namespace descriptors
    {
        // maximal x,y,roll,pitch,yaw obtained from the trajectories; rounded to two decimals
        const std::vector<double> max_deltas = {1.05, 1.15, 1.05, 1.55, 1.27}; 
        struct DescriptorBase
        {
        public:
            using robot_t = std::shared_ptr<Rhex>;

            template <typename Simu, typename robot>
            void operator()(Simu &simu, std::shared_ptr<robot> rob, const Eigen::Vector6d &init_trans)
            {
                assert(false);
            }

            template <typename T>
            void get(T &results)
            {
                assert(false);
            }
        };

        struct DutyCycle : public DescriptorBase
        {
        public:
            DutyCycle()
            {
                for (size_t i = 0; i < 6; i++)
                    _contacts[i] = std::vector<size_t>();
            }

            // sometimes the leg tip can be raised and the segment before it in collision with the ground,
            // thus check all the segments in the leg for collision with the gound, if collision detected
            // then record it and don't continue checking the rest of the leg.
            template <typename Simu, typename robot>
            void operator()(Simu &simu, std::shared_ptr<robot> rob, const Eigen::Vector6d &init_trans)
            {
                const dart::collision::CollisionResult &col_res = simu.world()->getLastCollisionResult();
                for (size_t i = 0; i < 6; ++i)
                {
                    if (rob->is_broken(i))
                    {
                        _contacts[i].push_back(0);
                    }
                    else
                    {
                        for (size_t j = 1; j <= 8; ++j)
                        {
                            std::string leg_segment = "leg_" + std::to_string(i) + "_" + std::to_string(j);
                            dart::dynamics::BodyNodePtr segment_to_check = rob->skeleton()->getBodyNode(leg_segment);
                            if (col_res.inCollision(segment_to_check))
                            {
                                _contacts[i].push_back(col_res.inCollision(segment_to_check));
                                break;
                            }

                            if (j == 8)
                            {
                                _contacts[i].push_back(0);
                            }
                        }
                    }
                }
            }

            void get(std::vector<double> &results)
            {
                for (size_t i = 0; i < 6; i++)
                {
                    results.push_back(std::round(std::accumulate(_contacts[i].begin(), _contacts[i].end(), 0.0) / double(_contacts[i].size()) * 100.0) / 100.0);
                }
            }

        protected:
            std::map<size_t, std::vector<size_t>> _contacts;
        };

        // intended leg phases, these are not observed behaviourally
        struct ControlPhase : public DescriptorBase
        {
        public:
            ControlPhase() {}

            template <typename Simu, typename robot>
            void operator()(Simu &simu, std::shared_ptr<robot> rob, const Eigen::Vector6d &init_trans)
            {
                std::vector<double> param = simu.controller().parameters();
                _phases = std::vector<double>(param.begin() + 18, param.end());
            }

            void get(std::vector<double> &results)
            {
                results = _phases;
            }

        protected:
            std::vector<double> _phases;
        };

        // describes endurance, the lower, the better.
        struct SpecificResistance : public DescriptorBase
        {
        public:
            SpecificResistance()
            {
                _powers = std::vector<double>();
                _velocities = std::vector<double>();
            }

            template <typename Simu, typename robot>
            void operator()(Simu &simu, std::shared_ptr<robot> rob, const Eigen::Vector6d &init_trans)
            {

                _mass = rob->skeleton()->getMass();
                _gravity = rob->skeleton()->getGravity();

                Eigen::Vector3d velocity = rob->skeleton()->getCOMLinearVelocity();

                // this is the direction rhex walks in
                _velocities.push_back(velocity[0]);
                Eigen::VectorXd state = rob->skeleton()->getForces().array().abs() * simu.world()->getTimeStep();
                _powers.push_back(state.sum());
            }

            void get(double &result)
            {
                double avg_vel = std::accumulate(_velocities.begin(), _velocities.end(), 0.0) / _velocities.size();
                double avg_pow = std::accumulate(_powers.begin(), _powers.end(), 0.0) / _powers.size();

                result = -1 * avg_pow / (_mass * _gravity[2] * avg_vel);
            }

        protected:
            std::vector<double> _velocities;
            std::vector<double> _powers;
            double _mass;
            Eigen::Vector3d _gravity;
        };

        // average velocity in xyz directions
        struct AvgCOMVelocities : public DescriptorBase
        {
        public:
            AvgCOMVelocities() {}

            template <typename Simu, typename robot>
            void operator()(Simu &simu, std::shared_ptr<robot> rob, const Eigen::Vector6d &init_trans)
            {
                Eigen::Vector3d velocity = rob->skeleton()->getCOMLinearVelocity();
                _vel_x.push_back(velocity[0]);
                _vel_y.push_back(velocity[1]);
                _vel_z.push_back(velocity[2]);
            }

            void get(Eigen::Vector3d &results)
            {
                results[0] = std::accumulate(_vel_x.begin(), _vel_x.end(), 0.0) / _vel_x.size();
                results[1] = std::accumulate(_vel_y.begin(), _vel_y.end(), 0.0) / _vel_y.size();
                results[2] = std::accumulate(_vel_z.begin(), _vel_z.end(), 0.0) / _vel_z.size();
            }

        protected:
            std::vector<double> _vel_x, _vel_y, _vel_z;
        };

        struct PositionTraj : public DescriptorBase
        {
        public:
            PositionTraj() {}

            template <typename Simu, typename robot>
            void operator()(Simu &simu, std::shared_ptr<robot> rob, const Eigen::Vector6d &init_trans)
            {
                Eigen::Vector6d pose = rob->pose();
                Eigen::Matrix3d rot = dart::math::expMapRot({pose[0], pose[1], pose[2]});
                Eigen::Matrix3d init_rot = dart::math::expMapRot({init_trans[0], init_trans[1], init_trans[2]});
                Eigen::MatrixXd init_homogeneous(4, 4);
                init_homogeneous << init_rot(0, 0), init_rot(0, 1), init_rot(0, 2), init_trans[3], init_rot(1, 0), init_rot(1, 1), init_rot(1, 2), init_trans[4], init_rot(2, 0), init_rot(2, 1), init_rot(2, 2), init_trans[5], 0, 0, 0, 1;
                Eigen::MatrixXd final_homogeneous(4, 4);
                final_homogeneous << rot(0, 0), rot(0, 1), rot(0, 2), pose[3], rot(1, 0), rot(1, 1), rot(1, 2), pose[4], rot(2, 0), rot(2, 1), rot(2, 2), pose[5], 0, 0, 0, 1;
                Eigen::Vector4d pos = {init_trans[3], init_trans[4], init_trans[5], 1.0};
                pos = init_homogeneous.inverse() * final_homogeneous * pos;

                _pos_traj.push_back({pos[0], pos[1], pos[2]});
            }

            void get(std::vector<Eigen::Vector3d> &results)
            {
                results = _pos_traj;
            }

        protected:
            std::vector<Eigen::Vector3d> _pos_traj;
        };
        struct RotationTraj : public DescriptorBase
        {
        public:
            RotationTraj() {}

            template <typename Simu, typename robot>
            void operator()(Simu &simu, std::shared_ptr<robot> rob, const Eigen::Vector6d &init_trans)
            {
                // roll-pitch-yaw
                Eigen::Matrix3d rot = dart::math::expMapRot(rob->rot());
                Eigen::Matrix3d init_rot = dart::math::expMapRot({init_trans[0], init_trans[1], init_trans[2]});
                auto rpy = dart::math::matrixToEulerXYZ(init_rot.inverse() * rot);

                _rotation_traj.push_back(rpy);
            }

            void get(std::vector<Eigen::Vector3d> &results)
            {
                results = _rotation_traj;
            }

        protected:
            std::vector<Eigen::Vector3d> _rotation_traj;
        };
        // centre-of-mass + rpy trajectory
        struct FullTrajectory : public DescriptorBase
        {
        public:
            FullTrajectory() {}

            template <typename Simu, typename robot>
            void operator()(Simu &simu, std::shared_ptr<robot> rob, const Eigen::Vector6d &init_trans)
            {
                this->_pos_traj(simu, rob, init_trans);
                this->_rot_traj(simu, rob, init_trans);
            }

            void get(std::vector<double> &results)
            {
                std::vector<Eigen::Vector3d> positions, rotations;
                this->_pos_traj.get(positions);
                this->_rot_traj.get(rotations);
                for (size_t i = 0; i < positions.size(); ++i)
                {

                    _traj.push_back(positions[i][0]);
                    _traj.push_back(positions[i][1]);
                    _traj.push_back(rotations[i][0]);
                    _traj.push_back(rotations[i][1]);
                    _traj.push_back(rotations[i][2]);
#ifdef GRAPHIC
                    std::ofstream ofs("/home/david/RHex_experiments/Results/fulltrajectory_log.txt", std::ios::app);
                    ofs << *(_traj.end() - 5) << " " << *(_traj.end() - 4) << " " << *(_traj.end() - 3) << " " << *(_traj.end() - 2) << " " << *(_traj.end() - 1) << std::endl;
#endif
                }
            }

        protected:
            PositionTraj _pos_traj;
            RotationTraj _rot_traj;
            std::vector<double> _traj;
        };

        // centre-of-mass + rpy trajectory
        struct DeltaFullTrajectory : public DescriptorBase
        {
        public:
            DeltaFullTrajectory() {}

            template <typename Simu, typename robot>
            void operator()(Simu &simu, std::shared_ptr<robot> rob, const Eigen::Vector6d &init_trans)
            {
                this->_pos_traj(simu, rob, init_trans);
                this->_rot_traj(simu, rob, init_trans);
            }

            void get(std::vector<double> &results)
            {
                std::vector<Eigen::Vector3d> positions, rotations;
                this->_pos_traj.get(positions);
                this->_rot_traj.get(rotations);
                std::vector<double> temp;
                for (size_t i = 0; i < positions.size(); ++i)
                {
                    temp.push_back(positions[i][0]);
                    temp.push_back(positions[i][1]);
                    temp.push_back(rotations[i][0]);
                    temp.push_back(rotations[i][1]);
                    temp.push_back(rotations[i][2]);
                }

                for (size_t i=5; i < temp.size(); i+=5 )
                {
                    for(size_t j=0; j<5; ++j)
                    {
                        _traj.push_back(_get_delta(temp[i+j-5],temp[i+j],j));
                    }
                }
            }
            

        protected:
            PositionTraj _pos_traj;
            RotationTraj _rot_traj;
            std::vector<double> _traj;

            double _get_delta(double before, double after, size_t index)
            {
                double delta = (after - before) / max_deltas[index];//[-1,1]
                delta = (1.0 + delta)/2.0;//[0,1]
                return std::min(1.0,std::max(0.0,delta));//clip
            }
        };

        struct BodyOrientation : public DescriptorBase
        {
        public:
            BodyOrientation() {}

            template <typename Simu, typename robot>
            void operator()(Simu &simu, std::shared_ptr<robot> rob, const Eigen::Vector6d &init_trans)
            {
                // roll-pitch-yaw
                Eigen::Matrix3d rr = dart::math::expMapRot(rob->rot());
                Eigen::Matrix3d ro = dart::math::expMapRot({init_trans[0], init_trans[1], init_trans[2]});
                auto rpy = dart::math::matrixToEulerXYZ(ro.inverse() * rr);

                _roll_vec.push_back(rpy(0));
                _pitch_vec.push_back(rpy(1));
                _yaw_vec.push_back(rpy(2));
            }

            void get(std::vector<double> &results)
            {
                double threshold = (_perc_threshold / 100.0) * dart::math::constants<double>::pi();
                results.clear();
                results.push_back(std::round(std::count_if(_roll_vec.begin(), _roll_vec.end(), [&threshold](double i) { return i > threshold; }) / double(_roll_vec.size()) * 100.0) / 100.0);
                results.push_back(std::round(std::count_if(_roll_vec.begin(), _roll_vec.end(), [&threshold](double i) { return i < -threshold; }) / double(_roll_vec.size()) * 100.0) / 100.0);
                results.push_back(std::round(std::count_if(_pitch_vec.begin(), _pitch_vec.end(), [&threshold](double i) { return i > threshold; }) / double(_pitch_vec.size()) * 100.0) / 100.0);
                results.push_back(std::round(std::count_if(_pitch_vec.begin(), _pitch_vec.end(), [&threshold](double i) { return i < -threshold; }) / double(_pitch_vec.size()) * 100.0) / 100.0);
                results.push_back(std::round(std::count_if(_yaw_vec.begin(), _yaw_vec.end(), [&threshold](double i) { return i > threshold; }) / double(_yaw_vec.size()) * 100.0) / 100.0);
                results.push_back(std::round(std::count_if(_yaw_vec.begin(), _yaw_vec.end(), [&threshold](double i) { return i < -threshold; }) / double(_yaw_vec.size()) * 100.0) / 100.0);
            }

        protected:
            // We count the time the robot's root orientation has exceeded a threshold angle in every direction.
            // This threshold angle is _perc_threshold*pi (empirically chosen).
            const double _perc_threshold = 0.5;

            std::vector<double> _roll_vec, _pitch_vec, _yaw_vec;
        };
    } // namespace descriptors
} // namespace rhex_dart

#endif
