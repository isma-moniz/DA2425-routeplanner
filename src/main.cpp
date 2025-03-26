#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "graph.hpp"

Graph<int> graph;

void showMenu() {
    std::cout << "\n=== Route Planning Tool ===" << std::endl;
    std::cout << "1. Carregar ficheiro Locations (data/Locations.csv)" << std::endl;
    std::cout << "2. Carregar ficheiro Distances (data/Distances.csv)" << std::endl;
    std::cout << "3. Calcular melhor rota (driving)" << std::endl;
    std::cout << "4. Calcular rota alternativa (driving)" << std::endl;
    std::cout << "5. Calcular rota com restrições (driving)" << std::endl;
    std::cout << "6. Calcular rota ambiental (driving + walking)" << std::endl;
    std::cout << "0. Sair" << std::endl;
    std::cout << "Escolha uma opção: ";
}

void loadLocations() {
    std::ifstream file("data/Locations.csv");
    std::string line;
    getline(file, line); // skip header

    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string name, idStr, code, parkingStr;
        getline(ss, name, ',');
        getline(ss, idStr, ',');
        getline(ss, code, ',');
        getline(ss, parkingStr, ',');

        int id = std::stoi(idStr);
        bool parking = (parkingStr == "1");
        graph.addVertex(id, code, parking);

        std::cout << "Location: " << name << " (ID: " << id << ", Code: " << code << ", Parking: " << parkingStr << ")" << std::endl;
    }

    std::cout << "Locations carregados com sucesso." << std::endl;
}

void loadDistances() {
    std::ifstream file("data/Distances.csv");
    std::string line;
    getline(file, line); // skip header

    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string loc1, loc2, drive, walk;

        getline(ss, loc1, ',');
        getline(ss, loc2, ',');
        getline(ss, drive, ',');
        getline(ss, walk, ',');

        int id1 = std::stoi(loc1);
        int id2 = std::stoi(loc2);

        double driveTime = (drive == "X") ? -1 : std::stod(drive);
        double walkTime = std::stod(walk);

        if (driveTime != -1) {
            graph.addBidirectionalEdge(id1, id2, walkTime, driveTime);
            std::cout << "Segmento: " << id1 << " <-> " << id2 << " | Driving: " << driveTime << " | Walking: " << walkTime << std::endl;
        } else {
            graph.addBidirectionalEdge(id1, id2, walkTime, -1);
            std::cout << "Segmento pedonal: " << id1 << " <-> " << id2 << " | Walking: " << walkTime << std::endl;
        }
    }

    std::cout << "Distances carregados com sucesso." << std::endl;
}

void calculateBestRoute() {
    std::cout << "[PLACEHOLDER] Função para calcular a melhor rota (driving)." << std::endl;
}

void calculateAlternativeRoute() {
    std::cout << "[PLACEHOLDER] Função para calcular a rota alternativa (driving)." << std::endl;
}

void calculateRestrictedRoute() {
    std::cout << "[PLACEHOLDER] Função para calcular rota com restrições (driving)." << std::endl;
}

void calculateEnvironmentalRoute() {
    std::cout << "[PLACEHOLDER] Função para calcular rota ambiental (driving + walking)." << std::endl;
}

int main() {
    int opcao;

    do {
        showMenu();
        std::cin >> opcao;

        switch (opcao) {
            case 1:
                loadLocations();
                break;
            case 2:
                loadDistances();
                break;
            case 3:
                calculateBestRoute();
                break;
            case 4:
                calculateAlternativeRoute();
                break;
            case 5:
                calculateRestrictedRoute();
                break;
            case 6:
                calculateEnvironmentalRoute();
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
