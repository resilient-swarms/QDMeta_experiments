
#ifndef FUNEXP_FEATURE_VEC_TYPES_HPP
#define FUNEXP_FEATURE_VEC_TYPES_HPP

#define LINEAR 0
#define SELECTION 1
#define NONLINEAR 2
#include <Eigen/Dense>
#define EIGEN_DENSEBASE_PLUGIN "EigenDenseBaseAddons.h"

namespace boost{
    namespace serialization{

        template<   class Archive, 
                    class S, 
                    int Rows_, 
                    int Cols_, 
                    int Ops_, 
                    int MaxRows_, 
                    int MaxCols_>
        inline void save(
            Archive & ar, 
            const Eigen::Matrix<S, Rows_, Cols_, Ops_, MaxRows_, MaxCols_> & g, 
            const unsigned int version)
            {
                int rows = g.rows();
                int cols = g.cols();

                ar & rows;
                ar & cols;
                ar & boost::serialization::make_array(g.data(), rows * cols);
            }

        template<   class Archive, 
                    class S, 
                    int Rows_,
                    int Cols_,
                    int Ops_, 
                    int MaxRows_, 
                    int MaxCols_>
        inline void load(
            Archive & ar, 
            Eigen::Matrix<S, Rows_, Cols_, Ops_, MaxRows_, MaxCols_> & g, 
            const unsigned int version)
        {
            int rows, cols;
            ar & rows;
            ar & cols;
            g.resize(rows, cols);
            ar & boost::serialization::make_array(g.data(), rows * cols);
        }

        template<   class Archive, 
                    class S, 
                    int Rows_, 
                    int Cols_, 
                    int Ops_, 
                    int MaxRows_, 
                    int MaxCols_>
        inline void serialize(
            Archive & ar, 
            Eigen::Matrix<S, Rows_, Cols_, Ops_, MaxRows_, MaxCols_> & g, 
            const unsigned int version)
        {
            split_free(ar, g, version);
        }


    } // namespace serialization
} // namespace boost



const int NUM_BASE_FEATURES = RASTRI_DIM; 
// const int NUM_TOP_CELLS = 15;      // number of cells in the meta-map
const int NUM_BOTTOM_FEATURES = 2; // number of features for bottom level maps

/* base-features */
typedef Eigen::Matrix<float, NUM_BASE_FEATURES, 1, Eigen::DontAlign, NUM_BASE_FEATURES, 1> base_features_t;       // 0 options and size cannot grow
typedef Eigen::Matrix<size_t, NUM_BOTTOM_FEATURES, 1, Eigen::DontAlign, NUM_BOTTOM_FEATURES, 1> bottom_indices_t; // 0 options and size cannot grow

#if FEATUREMAP == NONLINEAR

const int NUM_HIDDEN = 10; //number of hidden units
#if ADDITIONAL_GENES > 0
const int NUM_GENES = NUM_BASE_FEATURES * NUM_HIDDEN + NUM_HIDDEN * NUM_BOTTOM_FEATURES + 2 + ADDITIONAL_GENES;
#else
const int NUM_GENES = NUM_BASE_FEATURES * NUM_HIDDEN + NUM_HIDDEN * NUM_BOTTOM_FEATURES + 2;
#endif
/* weights to construct bottom-level map features from base_features */
typedef Eigen::Matrix<float, NUM_HIDDEN, NUM_BASE_FEATURES, Eigen::DontAlign, NUM_HIDDEN, NUM_BASE_FEATURES> weight1_t;
typedef Eigen::Matrix<float, NUM_BOTTOM_FEATURES, NUM_HIDDEN, Eigen::DontAlign, NUM_BOTTOM_FEATURES, NUM_HIDDEN> weight2_t;
typedef Eigen::Matrix<float, NUM_HIDDEN, 1, Eigen::DontAlign, NUM_HIDDEN, 1> hidden_t;
struct Weights
{
    weight1_t W1;
    weight2_t W2;
    float B1, B2;
};
typedef Weights weight_t;
#else

#if ADDITIONAL_GENES > 0
const int NUM_GENES = NUM_BASE_FEATURES * NUM_BOTTOM_FEATURES + ADDITIONAL_GENES;
#else
const int NUM_GENES = NUM_BASE_FEATURES * NUM_BOTTOM_FEATURES;
#endif
/* weights to construct bottom-level map features from base_features */
typedef Eigen::Matrix<float, NUM_BOTTOM_FEATURES, NUM_BASE_FEATURES, Eigen::DontAlign, NUM_BOTTOM_FEATURES, NUM_BASE_FEATURES> weight_t;
#endif

/* bottom-level map features */
typedef Eigen::Matrix<float, NUM_BOTTOM_FEATURES, 1, Eigen::DontAlign, NUM_BOTTOM_FEATURES, 1> bottom_features_t;

#endif
