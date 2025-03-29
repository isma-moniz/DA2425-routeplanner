#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <string>
#include "graph.hpp"

class StorageHandler {
public:
    void loadLocations(const std::string& locationsFile);
    void loadRoads(const std::string& roadFile);

    void callDijkstra(const std::string& source, const std::string& dest);
    void callRestrictedDijkstra(const std::string& src, const std::string& dest, 
        const std::string& avoidNodes, const std::string& avoidSegments, const std::string& includeNode);
    void calculateEnvironmentalRoute(int source, int destination, int maxWalkingTime);

private:
    Graph<int> cityGraph;
};

#endif
