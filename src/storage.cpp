#include "storage.hpp"
#include <limits>
#include <stdexcept>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <regex>

bool isNumeric(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

void StorageHandler::loadLocations(const std::string& locationsFile) {
    std::ifstream file(locationsFile);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open locations file " + locationsFile);
    }

    std::string line;
    getline(file, line); // ignore file header
    
    int linenumber = 2; // first data line is the second in file
    while (getline(file, line)) {
        linenumber++;
        std::stringstream ss(line);
        std::string locationName, idStr, code, parkingStr;
        
        if (!getline(ss, locationName, ',') ||
            !getline(ss, idStr, ',') ||
            !getline(ss, code, ',') ||
            !getline(ss, parkingStr, ',')) {
            std::cerr << "Warning: Skipping malformed line " << std::to_string(linenumber) << ": " << line << "\n";
            continue;
        }
        
        if (code.empty()) {
            std::cerr << "Warning: Skipping line " << std::to_string(linenumber) << " due to empty location code\n";
            continue;
        }
        
        try {
            int id = std::stoi(idStr);
            parkingStr.erase(std::remove_if(parkingStr.begin(), parkingStr.end(), ::isspace), parkingStr.end());
            bool parking = (parkingStr == "1");


            if (!cityGraph.addVertex(id, code, parking)) {
                throw std::runtime_error("Error adding vertex for ID " + idStr + " on line "  + std::to_string(linenumber));
            }
        } catch (const std::invalid_argument& e) {
            std::cerr << "Error: Invalid ID format on line " << std::to_string(linenumber) << ": " << idStr << "\n";
            continue;
        } catch (const std::exception& e) {
            std::cerr << "Error processing line " << std::to_string(linenumber) << ": " << e.what() << "\n";
            continue;
        }
    }

    file.close();
    std::cout << "Locations loaded successfully!\n";
}

void StorageHandler::loadRoads(const std::string& roadFile) {
    std::ifstream file(roadFile);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open roads file " + roadFile);
    }

    std::string line;
    getline(file, line); // ignore file header
    
    int linenumber = 2; // first data line is the second in file
    while (getline(file, line)) {
        linenumber++;
        std::stringstream ss(line);
        std::string loc1, loc2, drivingStr, walkingStr;
        
        if (!getline(ss, loc1, ',') ||
            !getline(ss, loc2, ',') ||
            !getline(ss, drivingStr, ',') ||
            !getline(ss, walkingStr, ',')) {
            std::cerr << "Warning: Skipping malformed line " << std::to_string(linenumber) << ": " << line << "\n";
            continue;
        }
        
        if (loc1.empty() || loc2.empty()) {
            std::cerr << "Warning: Skipping line " << std::to_string(linenumber) << " due to empty location.\n";
            continue;
        }
        
        try {
            double driving, walking;

            if (drivingStr == "X") {
                driving = std::numeric_limits<double>::max();
            } else {
                driving = std::stod(drivingStr);
            }

            if (walkingStr == "X") {
                walking = std::numeric_limits<double>::max();
            } else {
                walking = std::stod(walkingStr);
            }


            if (!cityGraph.addBidirectionalEdge(loc1, loc2, walking, driving)) {
                throw std::runtime_error("Error adding road for ID " + loc1 + "<-> " + loc2 + " on line "  + std::to_string(linenumber));
            }
        } catch (const std::invalid_argument& e) {
            std::cerr << "Error: Invalid format on line " << std::to_string(linenumber) << "\n";
            continue;
        } catch (const std::exception& e) {
            std::cerr << "Error processing line " << std::to_string(linenumber) << ": " << e.what() << "\n";
            continue;
        }
    }

    file.close();
    std::cout << "Locations loaded successfully!\n";
}

void StorageHandler::callDijkstra(const std::string& src, const std::string& dest) {
    int source, destination;
    if (!isNumeric(src)) {
        auto srcVert = cityGraph.findVertex(src);
        if (srcVert == nullptr) {
            throw std::runtime_error("Error: Vertex with code " + src + " not found!\n");
        }
        source = srcVert->getInfo();
    } else {
        source = std::stoi(src);
    }

    if (!isNumeric(dest)) {
        auto destVert = cityGraph.findVertex(dest);
        if (destVert == nullptr) {
            throw std::runtime_error("Error: Vertex with code " + dest + " not found!\n");
        }
        destination = destVert->getInfo();
    } else {
        destination = std::stoi(dest);
    }

    cityGraph.fastestDrivingPathWithAlt(source, destination);
}



void StorageHandler::callRestrictedDijkstra(const std::string& src, const std::string& dest, 
    const std::string& avoidNodes, const std::string& avoidSegments, const std::string& includeNode) {

        int source, destination;
        std::vector<int> avoidNodesSet;
        std::vector<std::pair<int,int>> avoidSegmentsSet;
        std::optional<int> stop;

        if (!isNumeric(src)) {
            auto srcVert = cityGraph.findVertex(src);
            if (srcVert == nullptr) {
                throw std::runtime_error("Error: Vertex with code " + src + " not found!\n");
            }
            source = srcVert->getInfo();
        } else {
            source = std::stoi(src);
        }

        if (!isNumeric(dest)) {
            auto destVert = cityGraph.findVertex(dest);
            if (destVert == nullptr) {
                throw std::runtime_error("Error: Vertex with code " + dest + " not found!\n");
            }
            destination = destVert->getInfo();
        } else {
            destination = std::stoi(dest);
        }

        // insert avoid nodes in nodeset
        if (!avoidNodes.empty()) {
            std::istringstream nodesStream(avoidNodes);
            std::string node;
            while (std::getline(nodesStream, node, ',')) {
                avoidNodesSet.push_back(std::stoi(node));
            }
        }

        // insert avoid segments in set
        if (!avoidSegments.empty()) {
            std::regex segment_regex(R"(\((\d+),(\d+)\))");
            auto segments_begin = std::sregex_iterator(avoidSegments.begin(), avoidSegments.end(), segment_regex);
            auto segments_end = std::sregex_iterator();

            for (auto it = segments_begin; it != segments_end; ++it) {
                int from = std::stoi((*it)[1].str());
                int to = std::stoi((*it)[2].str());
                avoidSegmentsSet.push_back({from, to});
            }
        }

        if (!includeNode.empty()) {  
            if (!isNumeric(includeNode)) {
                auto stopVert = cityGraph.findVertex(includeNode);
                if (stopVert == nullptr) {
                    throw std::runtime_error("Error: Vertex with code " + includeNode + " not found!\n");
                }
                stop= stopVert->getInfo();
            } else {
                stop = std::stoi(includeNode);
            }
        }
    std::cout << "Calling dijkstra!\n";
    cityGraph.fastestRestrictedDrivingPath(source, destination, avoidNodesSet, avoidSegmentsSet, stop);
}

void StorageHandler::calculateEnvironmentalRoute(int source, int destination, int maxWalkingTime) {
    std::vector<Vertex<int>*> parkingVertices = cityGraph.getAllParkingVertices();
    std::vector<std::tuple<double, std::vector<Edge<int>*>, std::vector<Edge<int>*>, int>> candidates;

    for (auto park : parkingVertices) {
        int parkId = park->getInfo();
        if (parkId == source || parkId == destination) continue;

        auto drivePath = cityGraph.dijkstraDriving(source, parkId);
        if (drivePath.empty()) continue;

        auto walkPath = cityGraph.dijkstraWalking(parkId, destination);
        if (walkPath.empty()) continue;

        double walkTime = 0;
        for (auto e : walkPath) walkTime += e->getWalkTime();
        if (walkTime > maxWalkingTime) continue;

        double driveTime = 0;
        for (auto e : drivePath) driveTime += e->getDriveTime();

        double totalTime = driveTime + walkTime;
        candidates.push_back({totalTime, drivePath, walkPath, parkId});
    }

    if (candidates.empty()) {
        std::cout << "Source:" << source << "\n";
        std::cout << "Destination:" << destination << "\n";
        std::cout << "DrivingRoute:none\n";
        std::cout << "ParkingNode:none\n";
        std::cout << "WalkingRoute:none\n";
        std::cout << "TotalTime:\n";
        std::cout << "Message: No valid path found with current constraints.\n";
        return;
    }

    auto best = *std::min_element(candidates.begin(), candidates.end(),
                                  [](const std::tuple<double, std::vector<Edge<int>*>, std::vector<Edge<int>*>, int>& a,
                                     const std::tuple<double, std::vector<Edge<int>*>, std::vector<Edge<int>*>, int>& b) {

                                      double totalA = std::get<0>(a);
                                      double totalB = std::get<0>(b);
                                      if (totalA != totalB) return totalA < totalB;

                                      double walkA = 0, walkB = 0;
                                      for (auto e : std::get<2>(a)) walkA += e->getWalkTime();
                                      for (auto e : std::get<2>(b)) walkB += e->getWalkTime();

                                      return walkA > walkB;
                                  });


    auto [totalTime, drivePath, walkPath, parkNode] = best;

    std::cout << "Source:" << source << "\n";
    std::cout << "Destination:" << destination << "\n";

    std::cout << "DrivingRoute:";
    for (auto e : drivePath) std::cout << e->getOrigin()->getInfo() << ",";
    std::cout << drivePath.back()->getDest()->getInfo() << "(";
    double driveTime = 0;
    for (auto e : drivePath) driveTime += e->getDriveTime();
    std::cout << static_cast<int>(driveTime) << ")\n";

    std::cout << "ParkingNode:" << parkNode << "\n";

    std::cout << "WalkingRoute:";
    for (auto e : walkPath) std::cout << e->getOrigin()->getInfo() << ",";
    std::cout << walkPath.back()->getDest()->getInfo() << "(";
    double walkTime = 0;
    for (auto e : walkPath) walkTime += e->getWalkTime();
    std::cout << static_cast<int>(walkTime) << ")\n";

    std::cout << "TotalTime:" << static_cast<int>(totalTime) << "\n";
}
