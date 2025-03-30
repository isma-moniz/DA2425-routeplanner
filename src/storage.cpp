#include "storage.hpp"
#include <limits>
#include <stdexcept>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <regex>

/**
 * @brief Checks if a string is a numeric representation.
 * 
 * This helper function determines whether a given string contains only digits.
 * 
 * @param str The string to check.
 * @return true if the string contains only digits, false otherwise.
 * 
 * @note The time complexity of this function is O(n), where n is the length of the string.
 *       The function iterates over each character in the string once to verify that they are all digits.
 */
bool isNumeric(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}


/**
 * @brief Loads locations from a CSV file.
 * 
 * This function loads location data from a CSV file and adds the locations as vertices to the `cityGraph`.
 * It parses each line of the file, extracts relevant fields (location name, ID, code, and parking status), and adds the parsed data as vertices in the graph.
 * 
 * @param locationsFile The file path of the locations CSV.
 * 
 * @throws std::runtime_error If the file cannot be opened or an error occurs during processing.
 * 
 * @note The time complexity of this function is O(m * n), where m is the number of lines in the file,
 *       and n is the average number of characters per line. Each line is parsed, and for each location, the algorithm
 *       attempts to add a vertex to the graph.
 */
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


/**
 * @brief Loads roads from a CSV file.
 * 
 * This function loads road data from a file and adds bidirectional roads as edges between vertices in the `cityGraph`.
 * It parses the file and handles both driving and walking road information for each road segment.
 * 
 * @param roadFile The file path of the roads CSV.
 * 
 * @throws std::runtime_error If the file cannot be opened or an error occurs during processing.
 * 
 * @note The time complexity of this function is O(m * n), where m is the number of lines in the file,
 *       and n is the average number of characters per line. For each line, the algorithm processes road data
 *       and adds bidirectional edges to the graph.
 */
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


/**
 * @brief Finds the shortest driving path between two locations using Dijkstra's algorithm.
 * 
 * This function attempts to find the shortest driving path between two locations, specified by either their vertex IDs or location codes.
 * It first checks if the provided source and destination are numeric (IDs) or location codes (strings), and converts them accordingly.
 * Once the valid source and destination IDs are obtained, it invokes the Dijkstra algorithm for the `cityGraph` to find the shortest path.
 * 
 * @param src The source location, either as an ID or a location code.
 * @param dest The destination location, either as an ID or a location code.
 * 
 * @throws std::runtime_error If either the source or destination is invalid or not found in the graph.
 * 
 * @note The time complexity of this function depends on the implementation of Dijkstra's algorithm:
 *       - If the graph is represented as an adjacency matrix, the time complexity is O(V^2), where V is the number of vertices.
 *       - If the graph is represented as an adjacency list and a priority queue (min-heap) is used, the time complexity is O((V + E) * log(V)),
 *         where V is the number of vertices and E is the number of edges in the graph.
 */
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




/**
 * @brief Finds the shortest driving path between two locations using Dijkstra's algorithm with additional restrictions.
 * 
 * This function finds the shortest driving path between two locations using Dijkstra's algorithm, with additional restrictions:
 * - It allows certain nodes to be avoided during the path calculation.
 * - It allows certain segments (edges) to be avoided.
 * - Optionally, it can require the path to pass through a specific node.
 * 
 * The function handles both location codes (strings) and IDs (numeric), and converts them as needed.
 * The function parses the comma-separated lists of nodes and segments to be avoided, as well as the optional node that must be included in the path.
 * 
 * @param src The source location, either as an ID or a location code.
 * @param dest The destination location, either as an ID or a location code.
 * @param avoidNodes A comma-separated string of nodes to avoid (IDs).
 * @param avoidSegments A comma-separated string of segments (edges) to avoid, represented as pairs of node IDs.
 * @param includeNode An optional node (ID or code) that must be included in the path.
 * 
 * @throws std::runtime_error If any of the specified nodes or edges are not found in the graph.
 * 
 * @note The time complexity of this function depends on the implementation of the restricted Dijkstra algorithm:
 *       - If the graph is represented as an adjacency matrix, the time complexity is O(V^2), where V is the number of vertices.
 *       - If the graph is represented as an adjacency list and a priority queue (min-heap) is used, the time complexity is O((V + E) * log(V)),
 *         where V is the number of vertices and E is the number of edges in the graph.
 *       - The additional restrictions (avoiding nodes, segments, and including a specific node) may add complexity, but the core complexity remains O((V + E) * log(V)) for an adjacency list representation.
 */
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


/**
 * @brief Calculates an environmental route from source to destination considering both driving and walking paths, with optional exclusions.
 * 
 * This function calculates the optimal route from a source to a destination while considering:
 * - Driving to a parking area, followed by walking to the destination.
 * - The ability to exclude certain nodes (locations) and segments (edges) from the graph during the calculation.
 * - The ability to filter routes based on a maximum walking time.
 * 
 * The algorithm first excludes the requested nodes and segments from the graph, then calculates potential routes by:
 * - Finding a driving route from the source to a parking location.
 * - Finding a walking route from that parking location to the destination.
 * 
 * The function stores the best routes that meet the conditions in a file, and outputs them in the console. If no valid route exists, an appropriate message is printed.
 * 
 * @param source The source node ID (integer).
 * @param destination The destination node ID (integer).
 * @param maxWalkingTime The maximum allowed walking time (in minutes).
 * @param avoidNodes A list of node IDs to be avoided during route calculation.
 * @param avoidSegments A list of edges (pairs of node IDs) to be avoided.
 * 
 * @note The function calculates all possible routes that satisfy the walking time constraint and returns the best one.
 *       If no valid route is found, the output will indicate that no route is possible.
 * 
 * @throws std::runtime_error If an invalid node or edge is encountered during processing.
 * 
 * @note The time complexity of this function can be broken down as follows:
 * - Excluding nodes and segments involves iterating through the provided lists (`O(n)` complexity, where `n` is the number of nodes or segments).
 * - For each parking node, two Dijkstra runs are performed:
 *   - One for driving from the source to the parking node (`O((V + E) * log(V))` for the adjacency list representation).
 *   - One for walking from the parking node to the destination (`O((V + E) * log(V))`).
 * - Sorting and selecting the best routes involves iterating through the possible routes, which is `O(m * log(m))`, where `m` is the number of candidates.
 * 
 * Overall, the time complexity of the function is dominated by the Dijkstra runs and sorting, making it approximately:
 * O(m * log(m) + 2 * (V + E) * log(V)).
 */
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


/**
 * @brief Parses a comma-separated string of integers and returns a vector of integers.
 * 
 * This function takes a string that represents a list of integers separated by commas, and converts it into a vector of integers. 
 * Each integer in the string is extracted, converted to an integer using `std::stoi`, and added to the resulting vector.
 * 
 * @param str The comma-separated string of integers.
 * 
 * @return A vector of integers extracted from the input string.
 * 
 * @note If the input string contains invalid or non-numeric data (such as letters or empty strings), the function will throw 
 *       a `std::invalid_argument` exception due to `std::stoi`. It is assumed that the input string is properly formatted for 
 *       integer parsing.
 * 
 * @complexity
 * - Time Complexity: O(N), where N is the number of characters in the input string. The function processes each character in the string,
 *   and for each comma-separated value, it calls `std::stoi`, which runs in constant time for each valid integer.
 *   - The overall complexity is dominated by the length of the input string as the string is split and parsed.
 * - Space Complexity: O(M), where M is the number of integers in the string. The space complexity is proportional to the number of integers 
 *   extracted and stored in the resulting vector.
 * 
 * @example
 * // Example usage:
 * std::vector<int> result = parseCommaSeparatedIntegers("1,2,3,4,5");
 * // result will contain {1, 2, 3, 4, 5}
 */

std::vector<int> StorageHandler::parseCommaSeparatedIntegers(const std::string& str) {
    std::vector<int> result;
    std::stringstream ss(str);
    std::string id;

    while (std::getline(ss, id, ',')) {
        result.push_back(std::stoi(id));
    }
    return result;
}

/**
 * @brief Parses a string containing pairs of integers enclosed in parentheses and returns a vector of integer pairs.
 * 
 * This function processes a string containing multiple pairs of integers formatted as `(a,b)`, where each pair is enclosed in parentheses
 * and separated by commas. The function uses regular expressions to extract these pairs and store them as `std::pair<int, int>` elements
 * in a vector.
 * 
 * @param str The input string containing pairs of integers in the form "(a,b)".
 * 
 * @return A vector of pairs, each containing two integers extracted from the input string.
 * 
 * @note The input string should adhere to the format `(a,b)` where `a` and `b` are integers. The function uses a regular expression to
 *       match pairs, and will throw an exception if the string is improperly formatted or if it contains invalid data.
 * 
 * @complexity
 * - Time Complexity: O(N), where N is the length of the input string. The function iterates through the string to find matching pairs using regular expressions.
 *   - The regular expression engine will scan the string linearly to find all matching pairs, and the complexity is proportional to the length of the string.
 *   - Each pair extraction operation (`std::stoi`) is constant time, but the overall complexity is dominated by the string length.
 * - Space Complexity: O(M), where M is the number of pairs found in the string. The space used depends on how many pairs of integers are parsed and stored in the vector.
 * 
 * @example
 * // Example usage:
 * std::vector<std::pair<int, int>> result = parsePairs("(1,2),(3,4),(5,6)");
 * // result will contain {{1, 2}, {3, 4}, {5, 6}}
 */

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


/**
 * @brief Parses the input file `input.txt` and extracts the batch input data into a `Data` structure.
 * 
 * This function reads the `input.txt` file, line by line, and extracts specific parameters based on the key-value format 
 * (e.g., "Mode: <value>"). It populates the provided `Data` structure with values for mode, source, destination, avoid nodes,
 * avoid segments, include node, and maximum walking time. If the file cannot be opened, or if the data format is invalid, 
 * an error code `-1` is returned.
 * 
 * The following keys are expected in the input file:
 * - `Mode`: Specifies the mode of operation.
 * - `Source`: The source vertex ID.
 * - `Destination`: The destination vertex ID.
 * - `AvoidNodes`: A comma-separated list of nodes to avoid.
 * - `AvoidSegments`: A list of pairs of nodes to avoid, formatted as "(node1,node2)".
 * - `IncludeNode`: A specific node to include in the route.
 * - `MaxWalkTime`: The maximum allowed walking time.
 * 
 * If any of these fields are missing or have invalid formats, the function returns `-1`. If the data is successfully 
 * parsed, the function returns `0`.
 * 
 * @param data A pointer to the `Data` structure where the parsed data will be stored.
 * 
 * @return Returns `0` on success, or `-1` if the input file format is invalid or the file cannot be opened.
 * 
 * @note The function expects the input file `input.txt` to be present in the project root directory. Each key in the file 
 *       should be followed by a colon (`:`) and then the corresponding value.
 * 
 * @complexity
 * - Time Complexity: O(N), where N is the number of lines in the input file. The function processes each line sequentially, 
 *   extracting key-value pairs. The complexity mainly depends on the number of lines in the file.
 *   - The `std::getline` function reads each line once, which is O(N).
 *   - Each line is parsed by splitting it into key and value, and this also operates in linear time relative to the line length.
 *   - The operations performed on each key-value pair (e.g., `std::stoi` or `parseCommaSeparatedIntegers`) are constant time operations 
 *     for small data sizes, though parsing large numbers of nodes or segments may add overhead.
 * - Space Complexity: O(M), where M is the total amount of data parsed. This depends on the number of entries in the input file 
 *   (e.g., the number of nodes in `AvoidNodes` or segments in `AvoidSegments`).
 * 
 * @example
 * // Example input.txt file:
 * Mode: Driving
 * Source: 1
 * Destination: 10
 * AvoidNodes: 2,3,5
 * AvoidSegments: (1,2),(3,4)
 * IncludeNode: 6
 * MaxWalkTime: 30
 * 
 * // Example usage:
 * Data data;
 * int result = parseBatchInput(&data);
 * if (result == 0) {
 *     // Data successfully parsed
 *     std::cout << "Source: " << data.source << std::endl;
 * } else {
 *     // Error reading the input file
 * }
 */
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


/**
 * @brief Calls the appropriate function based on the mode of operation specified in the `data` object.
 * 
 * This function determines the mode of operation (either "driving" or "driving-walking") from the `data` object and calls
 * the corresponding function to calculate the optimal route. It handles both normal and restricted driving paths, as well
 * as environmental route calculations considering walking time constraints.
 * 
 * - If the mode is "driving" and no nodes or segments are specified to avoid, it calls `fastestDrivingPathWithAlt` to find
 *   the fastest driving path.
 * - If the mode is "driving" and there are nodes, segments, or an include node specified, it calls `fastestRestrictedDrivingPath`
 *   to find the fastest path with restrictions based on the input.
 * - If the mode is "driving-walking", it calls `calculateEnvironmentalRoute` to find a route that combines driving and walking
 *   with walking time constraints.
 * 
 * @param data The `Data` structure containing the parameters to determine the type of path calculation.
 * 
 * @return void
 * 
 * @throws std::runtime_error If the mode in `data` is not recognized or if other parameters are invalid.
 * 
 * @complexity
 * - The complexity of `fastestDrivingPathWithAlt` depends on the underlying graph algorithm (e.g., Dijkstra), which typically
 *   has a time complexity of O((V + E) log V) where V is the number of vertices (nodes) and E is the number of edges.
 * - The complexity of `fastestRestrictedDrivingPath` is similar but may involve additional checks or constraints, possibly
 *   making it more costly depending on the number of avoid nodes/segments.
 * - `calculateEnvironmentalRoute` involves multiple graph searches (Dijkstra for driving and walking), each with a complexity
 *   of O((V + E) log V). The overall complexity will depend on the number of parking vertices and the number of paths considered.
 *   In the worst case, if there are N parking vertices, the complexity could be O(N * (V + E) log V).
 * 
 * @example
 * // Example usage for driving mode:
 * Data data;
 * data.mode = "driving";
 * data.source = 1;
 * data.destination = 10;
 * data.avoidNodes = {2, 3};
 * data.avoidSegments = {{1, 2}, {4, 5}};
 * data.includeNode = 6;
 * callBatchFunction(data);
 * 
 * // Example usage for driving-walking mode:
 * Data data;
 * data.mode = "driving-walking";
 * data.source = 1;
 * data.destination = 10;
 * data.maxWalkTime = 30;
 * data.avoidNodes = {2, 3};
 * data.avoidSegments = {{1, 2}, {4, 5}};
 * callBatchFunction(data);
 */
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