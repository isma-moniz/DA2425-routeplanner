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
    std::cout << "3. Calcular melhor rota (driving)\n";
    std::cout << "4. Calcular rota alternativa (driving)\n";
    std::cout << "5. Calcular rota com restrições (driving)\n";
    std::cout << "6. Calcular rota ambiental (driving + walking)\n";
    std::cout << "0. Sair\n";
    std::cout << "Escolha uma opção: ";
}

void calculateBestRoute() {
    std::string origem, destino;
    std::cout << "Código de origem: ";
    std::cin >> origem;
    std::cout << "Código de destino: ";
    std::cin >> destino;

    cityGraph.dijkstraDriving(origem);
    auto destVertex = cityGraph.findVertex(origem); // Usar o mesmo tipo para encontrar vértices

    if (!destVertex || destVertex->getDist() == std::numeric_limits<double>::max()) {
        std::cout << "Não foi possível encontrar uma rota." << std::endl;
        return;
    }

    auto destinationVertex = cityGraph.findVertex(destino);
    if (!destinationVertex || destinationVertex->getDist() == std::numeric_limits<double>::max()) {
        std::cout << "Não foi possível encontrar uma rota para o destino." << std::endl;
        return;
    }

    std::vector<std::string> path;
    auto curr = destinationVertex;
    while (curr && curr->getPath() != nullptr) {
        path.push_back(curr->getInfo());
        curr = curr->getPath()->getOrigin();
    }
    if (curr) path.push_back(curr->getInfo());

    std::reverse(path.begin(), path.end());

    std::cout << "Melhor rota (driving): ";
    for (const auto &code : path) std::cout << code << " ";
    std::cout << "| Tempo total: " << destinationVertex->getDist() << " minutos." << std::endl;
}

void placeholderFunction(const std::string &featureName) {
    std::cout << "Função '" << featureName << "' ainda não implementada." << std::endl;
}

int main() {
    int opcao;
    do {
        showMenu();
        std::cin >> opcao;
        switch (opcao) {
            case 1:
                loadLocations("../data/Locations.csv");
                break;
            case 2:
                loadDistances("../data/Distances.csv");
                break;
            case 3:
                calculateBestRoute();
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
                std::cout << "A sair..." << std::endl;
                break;
            default:
                std::cout << "Opção inválida." << std::endl;
        }
    } while (opcao != 0);

    return 0;
}
