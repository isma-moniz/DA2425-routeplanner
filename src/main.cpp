#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "graph.hpp" // Usa a tua estrutura de grafo

Graph<int> cityGraph;

void loadLocations(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Erro ao abrir ficheiro: " << filename << std::endl;
        return;
    }

    std::string line;
    getline(file, line); // Ignorar header
    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string locationName, idStr, code, parkingStr;
        getline(ss, locationName, ',');
        getline(ss, idStr, ',');
        getline(ss, code, ',');
        getline(ss, parkingStr, ',');

        int id = stoi(idStr);
        bool parking = (parkingStr == "1");

        cityGraph.addVertex(id, code, parking);
    }
    file.close();
    std::cout << "Locais carregados com sucesso!\n";
}

void loadDistances(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Erro ao abrir ficheiro: " << filename << std::endl;
        return;
    }

    std::string line;
    getline(file, line); // Ignorar header
    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string loc1, loc2, drivingStr, walkingStr;
        getline(ss, loc1, ',');
        getline(ss, loc2, ',');
        getline(ss, drivingStr, ',');
        getline(ss, walkingStr, ',');

        int source = stoi(loc1);
        int destination = stoi(loc2);

        double driveTime = (drivingStr == "X") ? -1 : stod(drivingStr);
        double walkTime = (walkingStr == "X") ? -1 : stod(walkingStr);

        cityGraph.addBidirectionalEdge(source, destination, walkTime, driveTime);
    }
    file.close();
    std::cout << "Distâncias carregadas com sucesso!\n";
}

void showMenu() {
    std::cout << "\n=== Route Planning Tool ===\n";
    std::cout << "1. Carregar Locations.csv\n";
    std::cout << "2. Carregar Distances.csv\n";
    std::cout << "3. Calcular melhor rota (driving)\n";
    std::cout << "4. Calcular rota alternativa (driving)\n";
    std::cout << "5. Calcular rota com restrições (driving)\n";
    std::cout << "6. Calcular rota ambiental (driving + walking)\n";
    std::cout << "0. Sair\n";
    std::cout << "Escolha uma opção: ";
}

void calculateBestRoute() {
    std::cout << "[PLACEHOLDER] Função para calcular a melhor rota.\n";
    // Aqui depois implementas Dijkstra ou o algoritmo escolhido.
}

void calculateAlternativeRoute() {
    std::cout << "[PLACEHOLDER] Função para calcular rota alternativa.\n";
}

void calculateRestrictedRoute() {
    std::cout << "[PLACEHOLDER] Função para rota com restrições.\n";
}

void calculateEcoRoute() {
    std::cout << "[PLACEHOLDER] Função para rota ambiental (driving + walking).\n";
}

int main() {
    int opcao;
    do {
        showMenu();
        std::cin >> opcao;

        switch (opcao) {
            case 1: {
                std::string file;
                std::cout << "Nome do ficheiro Locations.csv: ";
                std::cin >> file;
                loadLocations(file);
                break;
            }
            case 2: {
                std::string file;
                std::cout << "Nome do ficheiro Distances.csv: ";
                std::cin >> file;
                loadDistances(file);
                break;
            }
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
                calculateEcoRoute();
                break;
            case 0:
                std::cout << "A sair...\n";
                break;
            default:
                std::cout << "Opção inválida.\n";
        }
    } while (opcao != 0);

    return 0;
}
