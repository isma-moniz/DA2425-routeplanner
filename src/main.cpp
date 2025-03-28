#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <limits>
#include <queue>
#include "storage.hpp"

StorageHandler storageHandler;

void showMenu() {
    std::cout << "\n=== Route Planning Tool ===\n";
    std::cout << "1. Load Locations.csv\n";
    std::cout << "2. Load Distances.csv\n";
    std::cout << "3. Calculate best route (driving)\n";
    std::cout << "5. Calculate route with restrictions (driving)\n";
    std::cout << "6. Calculate environmentally friendly route (driving + walking)\n";
    std::cout << "0. Exit\n";
    std::cout << "Choose an option: ";
}

void placeholderFunction(const std::string &featureName) {
    std::cout << "Function '" << featureName << "' not implemented yet." << std::endl;
}

void calcBestRoute() {
    system("clear");
    std::string src;
    std::string dest;
    std::cout << "Enter source id or code: ";
    std::cin >> src;
    std::cout << "Enter dest id or code: ";
    std::cin >> dest;
    
    storageHandler.callDijkstra(src, dest);
}

int main() {
    int op;
    do {
        showMenu();
        std::cin >> op;
        switch (op) {
            case 1:
                storageHandler.loadLocations("../data/Locations.csv");
                break;
            case 2:
                storageHandler.loadRoads("../data/Distances.csv");
                break;
            case 3:
                calcBestRoute();
                break;
            case 4:
                placeholderFunction("Calcular rota alternativa");
                break;
            case 5:
                placeholderFunction("Calcular rota com restrições");
                break;
            case 6:
                placeholderFunction("Calcular rota ambiental");
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
