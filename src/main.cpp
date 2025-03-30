#include <iostream>
#include <string>
#include "storage.hpp"

StorageHandler storageHandler;

/**
 * @brief Displays the main menu of the route planning tool.
 * 
 * This function prints a menu with options for loading files, calculating routes, and running batch operations.
 * 
 * Time complexity: O(1), as this function only prints text and does not perform any significant computation.
 */
void showMenu() {
    std::cout << "\n=== Route Planning Tool ===\n";
    std::cout << "1. Load Locations.csv\n";
    std::cout << "2. Load Distances.csv\n";
    std::cout << "3. Calculate best route (driving)\n";
    std::cout << "4. Calculate route with restrictions (driving)\n";
    std::cout << "5. Calculate environmentally friendly route (driving + walking)\n";
    std::cout << "6. Use batch mode\n";
    std::cout << "0. Exit\n";
    std::cout << "Choose an option: ";
}

/**
 * @brief Placeholder function for unimplemented features.
 * 
 * This function displays a message indicating that the requested feature is not implemented yet.
 * 
 * @param featureName The name of the feature that is not implemented.
 * 
 * Time complexity: O(1), as it only prints a message.
 */
void placeholderFunction(const std::string &featureName) {
    std::cout << "Function '" << featureName << "' not implemented yet." << std::endl;
}

/**
 * @brief Calculates the best driving route between two locations using Dijkstra's algorithm.
 * 
 * This function asks the user for a source and destination location ID, then calls Dijkstra's algorithm 
 * via the `storageHandler` to calculate the best driving route.
 * 
 * Time complexity: O(E log V), where E is the number of edges and V is the number of vertices. 
 * This is due to the Dijkstra's algorithm used in `storageHandler.callDijkstra()`.
 */
void calcBestDrivingRoute() {
    system("clear");
    std::string src;
    std::string dest;
    std::cout << "Enter source id or code: ";
    std::cin >> src;
    std::cout << "Enter dest id or code: ";
    std::cin >> dest;
    
    storageHandler.callDijkstra(src, dest);
}

/**
 * @brief Calculates the best driving route with restrictions.
 * 
 * This function allows the user to input source and destination IDs, as well as avoid nodes and segments,
 * and then calls the restricted Dijkstra's algorithm to calculate the route with the given restrictions.
 * 
 * @param src The source location ID.
 * @param dest The destination location ID.
 * @param avoidNodes A string of node IDs to avoid, separated by commas.
 * @param avoidSegments A string of segment IDs to avoid, separated by commas.
 * @param includeNode A string representing a node to include in the route.
 * 
 * Time complexity: O(E log V), where E is the number of edges and V is the number of vertices.
 * The complexity is similar to that of Dijkstra's algorithm but with additional constraints.
 */
void calcDrivingRouteRestricted() {
    system("clear");
    std::string src, dest, avoidNodes, avoidSegments, includeNode;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Enter source id or code: ";
    std::getline(std::cin, src);

    std::cout << "Enter dest id or code: ";
    std::getline(std::cin, dest);

    std::cout << "Enter avoidNodes id separated by a comma ',': ";
    std::getline(std::cin, avoidNodes);

    std::cout << "Enter avoidSegments (id1,id2) separated by a comma ',': ";
    std::getline(std::cin, avoidSegments);

    std::cout << "Enter includeNode id: ";
    std::getline(std::cin, includeNode);

    storageHandler.callRestrictedDijkstra(src, dest, avoidNodes, avoidSegments, includeNode);
}

/**
 * @brief Calculates an environmentally friendly route that combines driving and walking.
 * 
 * This function asks the user for a source, destination, and the maximum allowed walking time, and then calls 
 * the method to calculate an environmentally friendly route using the given parameters.
 * 
 * Time complexity: O(E log V), where E is the number of edges and V is the number of vertices in the graph.
 * The complexity is similar to Dijkstra's algorithm for combined walking and driving.
 */
void calcEnvironmentalRoute() {
    system("clear");
    std::string src, dest;
    int maxWalk;
    std::vector<int> avoidNodesVec;
    std::vector<std::pair<int,int>> avoidSegmentsVec;

    std::cout << "Enter source id: ";
    std::cin >> src;
    std::cout << "Enter destination id: ";
    std::cin >> dest;
    std::cout << "Enter max walking time (in minutes): ";
    std::cin >> maxWalk;

    try {
        int source = std::stoi(src);
        int destination = std::stoi(dest);
        storageHandler.calculateEnvironmentalRoute(source, destination, maxWalk, avoidNodesVec, avoidSegmentsVec);
    } catch (...) {
        std::cerr << "Invalid input. Please enter valid numeric IDs.\n";
    }
}

/**
 * @brief Handles batch mode operations.
 * 
 * This function reads input from a file, parses it, and calls batch operations for route planning.
 * 
 * Time complexity: O(N), where N is the number of operations in the batch. This depends on the number of input lines to parse and process.
 */
void handleBatchMode() {
    Data data;
    if (storageHandler.parseBatchInput(&data) != 0) {
        std::cout << "Bad input.txt format\n";
        return;
    }
    storageHandler.callBatchFunction(data);
}

/**
 * @brief Main entry point for the route planning tool.
 * 
 * This function continuously displays the menu and processes user input for various operations such as loading
 * files, calculating routes, and running batch operations.
 * 
 * Time complexity: O(N), where N is the number of menu operations, depending on how many times the user interacts with the menu.
 */
int main() {
    int op;
    do {
        showMenu();
        std::cin >> op;
        switch (op) {
            case 1:
                storageHandler.loadLocations("../data/smallLoc.csv");
                break;
            case 2:
                storageHandler.loadRoads("../data/smallDist.csv");
                break;
            case 3:
                calcBestDrivingRoute();
                break;
            case 4:
                calcDrivingRouteRestricted();
                break;
            case 5:
                calcEnvironmentalRoute();
                break;
            case 6:
                handleBatchMode();
                break;
            case 0:
                std::cout << "Thank you for using route planner." << std::endl;
                break;
            default:
                std::cout << "Opção inválida." << std::endl;
        }
    } while (op != 0);

    return 0;
}
