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
            avoidNodesSet = parseCommaSeparatedIntegers(avoidNodes);
        }

        // insert avoid segments in set
        if (!avoidSegments.empty()) {
            avoidSegmentsSet = parsePairs(avoidSegments);
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

void StorageHandler::calculateEnvironmentalRoute(int source, int destination, int maxWalkingTime, std::vector<int> avoidNodes, std::vector<std::pair<int,int>> avoidSegments) {
    std::ofstream file("../output.txt");

    // exclude requested nodes
    for (auto node : avoidNodes) {
        Vertex<int>* vert = cityGraph.findVertex(node);
        if (vert == nullptr) {
            continue; // assume typo if not found
        }
        vert->setAvailable(false);
    }

    std::vector<Edge<int>*> switchedEdges; // we store edges here to switch them back to available without long lookups
    // exclude requested segments
    for (const auto& pair : avoidSegments) {
        Vertex<int>* orig = cityGraph.findVertex(pair.first);
        if (orig == nullptr || cityGraph.findVertex(pair.second) == nullptr) {
            continue;
        }

        for (auto edge : orig->getAdj()) { // this has time complexity O(e) and could be made faster if we used an unordered_map for adj,
            if (edge->getDest()->getInfo() == pair.second) { // but since the graph is not too big the performance gain would be negligible
                edge->setAvailable(false);
                switchedEdges.push_back(edge);
            }
        }
    }


    std::vector<Vertex<int>*> parkingVertices = cityGraph.getAllParkingVertices();
    std::vector<std::tuple<double, std::vector<Edge<int>*>, std::vector<Edge<int>*>, int>> candidates;
    std::vector<std::tuple<double, double, std::vector<Edge<int>*>, std::vector<Edge<int>*>, int>> approxCandidates;

    for (auto park : parkingVertices) {
        int parkId = park->getInfo();
        if (parkId == source || parkId == destination) continue;

        auto drivePath = cityGraph.dijkstraDriving(source, parkId);
        if (drivePath.empty()) continue;

        auto walkPath = cityGraph.dijkstraWalking(parkId, destination);
        if (walkPath.empty()) continue;

        double walkTime = 0;
        for (auto e : walkPath) walkTime += e->getWalkTime();

        double driveTime = 0;
        for (auto e : drivePath) driveTime += e->getDriveTime();

        double totalTime = driveTime + walkTime;

        if (walkTime <= maxWalkingTime) {
            candidates.push_back({totalTime, drivePath, walkPath, parkId});
        } else {
            approxCandidates.push_back({totalTime, walkTime, drivePath, walkPath, parkId});
        }
    }

    if (!candidates.empty()) {
        auto best = *std::min_element(candidates.begin(), candidates.end(),
                                      [](const std::tuple<double, std::vector<Edge<int>*>, std::vector<Edge<int>*>, int>& a,
                                         const std::tuple<double, std::vector<Edge<int>*>, std::vector<Edge<int>*>, int>& b) {
                                          return std::get<0>(a) < std::get<0>(b);
                                      });

        auto [totalTime, drivePath, walkPath, parkNode] = best;

        std::cout << "Source:" << source << "\n";
        file << "Source:" << source << "\n";
        std::cout << "Destination:" << destination << "\n";
        file << "Destination:" << destination << "\n";

        std::cout << "DrivingRoute:";
        file << "DrivingRoute:";
        for (auto e : drivePath) {std::cout << e->getOrigin()->getInfo() << ",";  file << e->getOrigin()->getInfo() << ",";}
        std::cout << drivePath.back()->getDest()->getInfo() << "(";
        file << drivePath.back()->getDest()->getInfo() << "(";
        double driveTime = 0;
        for (auto e : drivePath) driveTime += e->getDriveTime();
        std::cout << static_cast<int>(driveTime) << ")\n";
        file << static_cast<int>(driveTime) << ")\n";

        std::cout << "ParkingNode:" << parkNode << "\n";
        file << "ParkingNode:" << parkNode << "\n";

        std::cout << "WalkingRoute:";
        file << "WalkingRoute:";
        for (auto e : walkPath) { std::cout << e->getOrigin()->getInfo() << ","; file << e->getOrigin()->getInfo() << ",";}
        std::cout << walkPath.back()->getDest()->getInfo() << "(" << static_cast<int>(std::get<0>(best) - driveTime) << ")\n";
        file << walkPath.back()->getDest()->getInfo() << "(" << static_cast<int>(std::get<0>(best) - driveTime) << ")\n";
        std::cout << "TotalTime:" << static_cast<int>(totalTime) << "\n";
        file << "TotalTime:" << static_cast<int>(totalTime) << "\n";

        for (auto node : avoidNodes) {
            Vertex<int>* vert = cityGraph.findVertex(node);
            if (vert == nullptr) {
                continue; // assume typo if not found
            }
            vert->setAvailable(true);
        }

        for (auto edge : switchedEdges) edge->setAvailable(true);
        file.close();
        return;
    }

    if (!approxCandidates.empty()) {
        std::sort(approxCandidates.begin(), approxCandidates.end(),
                  [](const std::tuple<double, double, std::vector<Edge<int>*>, std::vector<Edge<int>*>, int>& a,
                     const std::tuple<double, double, std::vector<Edge<int>*>, std::vector<Edge<int>*>, int>& b) {
                      if (std::get<0>(a) != std::get<0>(b)) return std::get<0>(a) < std::get<0>(b);
                      return std::get<1>(a) < std::get<1>(b); // menor walkTime
                  });

        std::cout << "Source:" << source << "\n";
        std::cout << "Destination:" << destination << "\n";
        file << "Source:" << source << "\n";
        file << "Destination:" << destination << "\n";

        double bestTime = std::get<0>(approxCandidates[0]);
        int i = 1;
        for (auto& t : approxCandidates) {
            double totalTime = std::get<0>(t);
            if (totalTime > bestTime + 2) break; // ignora soluções muito piores

            double walkTime = std::get<1>(t);
            auto drivePath = std::get<2>(t);
            auto walkPath = std::get<3>(t);
            int parkNode = std::get<4>(t);

            std::cout << "DrivingRoute" << i << ":";
            file << "DrivingRoute" << i << ":";

            for (auto e : drivePath) {std::cout << e->getOrigin()->getInfo() << ","; file << e->getOrigin()->getInfo() << ",";}
            std::cout << drivePath.back()->getDest()->getInfo() << "(";
            file << drivePath.back()->getDest()->getInfo() << "(";
            double driveTime = 0;
            for (auto e : drivePath) driveTime += e->getDriveTime();
            std::cout << static_cast<int>(driveTime) << ")\n";
            file << static_cast<int>(driveTime) << ")\n";

            std::cout << "ParkingNode" << i << ":" << parkNode << "\n";
            file << "ParkingNode" << i << ":" << parkNode << "\n";

            std::cout << "WalkingRoute" << i << ":";
            file << "WalkingRoute" << i << ":";
            for (auto e : walkPath) {std::cout << e->getOrigin()->getInfo() << ","; file << e->getOrigin()->getInfo() << ",";}
            std::cout << walkPath.back()->getDest()->getInfo() << "(" << static_cast<int>(walkTime) << ")\n";

            std::cout << "TotalTime" << i << ":" << static_cast<int>(totalTime) << "\n";
            file << walkPath.back()->getDest()->getInfo() << "(" << static_cast<int>(walkTime) << ")\n";

            file << "TotalTime" << i << ":" << static_cast<int>(totalTime) << "\n";
            i++;
        }

        for (auto node : avoidNodes) {
            Vertex<int>* vert = cityGraph.findVertex(node);
            if (vert == nullptr) {
                continue; // assume typo if not found
            }
            vert->setAvailable(true);
        }

        for (auto edge : switchedEdges) edge->setAvailable(true);
        file.close();
        return;
    }

    std::cout << "Source:" << source << "\n";
    std::cout << "Destination:" << destination << "\n";
    std::cout << "DrivingRoute:none\n";
    std::cout << "ParkingNode:none\n";
    std::cout << "WalkingRoute:none\n";
    std::cout << "TotalTime:\n";
    std::cout << "No possible route with max. walking time of " << maxWalkingTime << " minutes.\n";

    file << "Source:" << source << "\n";
    file << "Destination:" << destination << "\n";
    file << "DrivingRoute:none\n";
    file << "ParkingNode:none\n";
    file << "WalkingRoute:none\n";
    file << "TotalTime:\n";
    file << "No possible route with max. walking time of " << maxWalkingTime << " minutes.\n";

    for (auto node : avoidNodes) {
        Vertex<int>* vert = cityGraph.findVertex(node);
        if (vert == nullptr) {
            continue; // assume typo if not found
        }
        vert->setAvailable(true);
    }

    for (auto edge : switchedEdges) edge->setAvailable(true);
    file.close();
    return;
}

std::vector<int> StorageHandler::parseCommaSeparatedIntegers(const std::string& str) {
    std::vector<int> result;
    std::stringstream ss(str);
    std::string id;

    while (std::getline(ss, id, ',')) {
        result.push_back(std::stoi(id));
    }
    return result;
}

std::vector<std::pair<int, int>> StorageHandler::parsePairs(const std::string& str) {
    std::vector<std::pair<int, int>> result;
    std::regex segment_regex(R"(\((\d+),(\d+)\))");
    auto segments_begin = std::sregex_iterator(str.begin(), str.end(), segment_regex);
    auto segments_end = std::sregex_iterator();
    
    for (auto it = segments_begin; it != segments_end; ++it) {
        int from = std::stoi((*it)[1].str());
        int to = std::stoi((*it)[2].str());
        result.push_back({from, to});
    }
    return result;
}

int StorageHandler::parseBatchInput(Data* data) {
    std::ifstream inputFile("../input.txt");
    std::string line;

    data->mode = "";
    data->source = -1;
    data->destination = -1;
    data->avoidNodes.clear();
    data->avoidSegments.clear();
    data->includeNode = -1;
    data->maxWalkTime = -1;

    if (!inputFile.is_open()) {
        throw std::runtime_error("File input.txt not found in project root.");
    }

    while (std::getline(inputFile, line)) {
        std::stringstream ss(line);
        std::string key, value;

        if (std::getline(ss, key, ':')) {
            try {
                std::getline(ss, value);
                if (value.empty()) continue;

                if (key == "Mode") {
                    data->mode = value;
                } else if (key == "Source") {
                    data->source = std::stoi(value);
                } else if (key == "Destination") {
                    data->destination = std::stoi(value);
                } else if (key == "AvoidNodes") {
                    data->avoidNodes = parseCommaSeparatedIntegers(value);
                } else if (key == "AvoidSegments") {
                    data->avoidSegments = parsePairs(value);
                } else if (key == "IncludeNode") {
                    data->includeNode = std::stoi(value);
                } else if (key == "MaxWalkTime") {
                    data->maxWalkTime = std::stoi(value);
                } else {
                    return -1; // ignore badly formatted input
                }
            } catch (std::invalid_argument) {
                return -1; // ignore badly formatted input
            }
            
        }
    }

    inputFile.close();
    return 0;
}

void StorageHandler::callBatchFunction(const Data& data) {
    if (data.mode == "driving") {
        if (data.avoidNodes.empty() && data.avoidSegments.empty() && data.includeNode == -1) {
            cityGraph.fastestDrivingPathWithAlt(data.source, data.destination);
        } else {
            cityGraph.fastestRestrictedDrivingPath(data.source, data.destination, data.avoidNodes,
                 data.avoidSegments, data.includeNode == -1 ? std::nullopt : std::optional<int>(data.includeNode));
        }
    } else if (data.mode == "driving-walking") {
        calculateEnvironmentalRoute(data.source, data.destination, data.maxWalkTime, data.avoidNodes, data.avoidSegments);
    }
}