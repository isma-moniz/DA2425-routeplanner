#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <string>
#include "graph.hpp"
#include <iostream>
#include <sstream>
#include <fstream>

class StorageHandler {
public:
    StorageHandler();
    void loadLocations(const std::string& locationsFile);
    void loadRoads(const std::string& roadFile);
private:
    Graph<int> cityGraph;
};

#endif
