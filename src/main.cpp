#include <iostream>
#include <string>
#include "storage.hpp"

StorageHandler storageHandler;

void showMenu() {
    std::cout << "\n=== Route Planning Tool ===\n";
    std::cout << "1. Load Locations.csv\n";
    std::cout << "2. Load Distances.csv\n";
    std::cout << "3. Calculate best route (driving)\n";
    std::cout << "4. Calculate route with restrictions (driving)\n";
    std::cout << "6. Calculate environmentally friendly route (driving + walking)\n";
    std::cout << "0. Exit\n";
    std::cout << "Choose an option: ";
}

void placeholderFunction(const std::string &featureName) {
    std::cout << "Function '" << featureName << "' not implemented yet." << std::endl;
}

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

void calcEnvironmentalRoute() {
    system("clear");
    std::string src, dest;
    int maxWalk;

    std::cout << "Enter source id: ";
    std::cin >> src;
    std::cout << "Enter destination id: ";
    std::cin >> dest;
    std::cout << "Enter max walking time (in minutes): ";
    std::cin >> maxWalk;

    try {
        int source = std::stoi(src);
        int destination = std::stoi(dest);
        storageHandler.calculateEnvironmentalRoute(source, destination, maxWalk);
    } catch (...) {
        std::cerr << "Invalid input. Please enter valid numeric IDs.\n";
    }
}



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
                placeholderFunction("Calcular rota com restrições");
                break;
            case 6:
                calcEnvironmentalRoute();
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
