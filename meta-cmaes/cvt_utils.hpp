#ifndef CVT_UTILS_HPP
#define CVT_UTILS_HPP

#ifdef TEST
#include <meta-cmaes/stat_map_cvtme.hpp>
#else
#include <modules/cvt_map_elites/stat_map.hpp>
#endif
#include <modules/cvt_map_elites/cvt_map_elites.hpp>

typedef BottomParams::ea::point_t point_t;

std::vector<point_t> load_centroids(const std::string &centroids_filename)
{
    std::vector<point_t> centroids;

    std::ifstream fin(centroids_filename.c_str());

    if (!fin)
    {
        std::cout << centroids_filename << std::endl;
        std::cerr << "Error: Could not load the centroids." << std::endl;
        exit(1);
    }

    std::vector<std::string> lines;

    std::string line;
    while (std::getline(fin, line))
    {
        if (!line.empty())
        {
            lines.push_back(line);
        }
    }

    fin.close();
    
    if (lines.size() != BottomParams::ea::number_of_clusters)
    {
        std::cerr << "Error: The number of clusters "
                  << BottomParams::ea::number_of_clusters
                  << " is not equal to the number of loaded elements "
                  << lines.size() << ".\n";
        exit(1);
    }
    //size_t num_cols;
    for (size_t i = 0; i < lines.size(); ++i)
    {
        std::vector<std::string> cols;

        std::string temp;
        std::istringstream stringStream;
        stringStream.str(lines[i]);

        while (stringStream >> temp)
            cols.push_back(temp);
        //num_cols = cols.size();
        //std::cout << "line "<< i << "num cols = "<< num_cols;
        if (cols.size() != BottomParams::ea::number_of_dimensions)
        {
            std::cerr << "Error: The number of dimensions "
                      << BottomParams::ea::number_of_dimensions
                      << " is not equal to the dimensionality (" << cols.size()
                      << ") of loaded element with index " << i << ".\n";
            exit(1);
        }

        point_t p;
        for (size_t j = 0; j < cols.size(); ++j)
            p[j] = atof(cols[j].c_str());

        centroids.push_back(p);
    }

    std::cout << "\nLoaded " << centroids.size() << " centroids.\n";

    return centroids;
}

#endif