#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <string>
#include "graph.hpp"

class StorageHandler {
public:
    void loadLocations(const std::string& locationsFile);
    void loadRoads(const std::string& roadFile);

    void callDijkstra(const std::string& source, const std::string& dest);
    void calculateEnvironmentalRoute(int source, int destination, int maxWalkingTime);

private:
    Graph<int> cityGraph;
};

#endif
