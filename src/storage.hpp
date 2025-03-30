#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <string>
#include "graph.hpp"

struct Data {
    std::string mode;
    int source;
    int destination;
    std::vector<int> avoidNodes;
    std::vector<std::pair<int,int>> avoidSegments;
    int includeNode;
    int maxWalkTime;
};

class StorageHandler {
public:
    void loadLocations(const std::string& locationsFile);
    void loadRoads(const std::string& roadFile);

    void callDijkstra(const std::string& source, const std::string& dest);
    void callRestrictedDijkstra(const std::string& src, const std::string& dest, 
        const std::string& avoidNodes, const std::string& avoidSegments, const std::string& includeNode);
    void calculateEnvironmentalRoute(int source, int destination, int maxWalkingTime);
    int parseBatchInput(Data* data);
    void callBatchFunction(const Data& data);

private:
    Graph<int> cityGraph;
    std::vector<int> parseCommaSeparatedIntegers(const std::string& str);
    std::vector<std::pair<int, int>> parsePairs(const std::string& str);
};

#endif
