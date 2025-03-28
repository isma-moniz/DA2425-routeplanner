#include "storage.hpp"

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
            int l1 = std::stoi(loc1);
            int l2 = std::stoi(loc2);
            double driving, walking;

            if (drivingStr == "X") {
                driving = std::numeric_limits<double>::max();
            } else if (walkingStr == "X") {
                walking = std::numeric_limits<double>::max();
            } else {
                driving = std::stod(drivingStr);
                walking = std::stod(walkingStr);
            }

            if (!cityGraph.addBidirectionalEdge(l1, l2, walking, driving)) {
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
