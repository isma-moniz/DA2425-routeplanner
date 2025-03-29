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
            } else if (walkingStr == "X") {
                walking = std::numeric_limits<double>::max();
            } else {
                driving = std::stod(drivingStr);
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