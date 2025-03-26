/**
 * @file main.cpp
 * @brief Main driver file for the Route Planning Tool.
 * 
 * This file contains the main program that interacts with the user, loads location and distance data from CSV files, 
 * and calculates the best route based on different criteria like driving or walking times.
 * 
 * The program uses the Graph class to represent the city and calculates optimal routes based on the user's input.
 */

 #include <iostream>
 #include <fstream>
 #include <sstream>
 #include <string>
 #include <vector>
 #include <limits>
 #include <queue>
 #include "graph.hpp"
 
 // Global city graph instance
 Graph<std::string> cityGraph;
 
 /**
  * @brief Loads location data from a CSV file and adds them as vertices in the graph.
  * 
  * This function reads a CSV file containing location information and creates corresponding vertices in the graph.
  * Each location's parking information is stored as part of the vertex data.
  * 
  * @param filename The path to the CSV file containing location data.
  * @return void
  * 
  * complexity O(N) where N is the number of locations in the CSV file.
  */
 void loadLocations(const std::string &filename) {
     std::ifstream file(filename);
     if (!file.is_open()) {
         std::cout << "Erro ao abrir ficheiro: " << filename << std::endl;
         return;
     }
 
     std::string line;
     getline(file, line); // Skip header
     while (getline(file, line)) {
         std::stringstream ss(line);
         std::string locationName, idStr, code, parkingStr;
         getline(ss, locationName, ',');
         getline(ss, idStr, ',');
         getline(ss, code, ',');
         getline(ss, parkingStr, ',');
 
         if (code.empty()) continue;
 
         bool parking = (parkingStr == "1");
         cityGraph.addVertex(code, parking);
     }
     file.close();
     std::cout << "Locais carregados com sucesso!\n";
 }
 
 /**
  * @brief Loads distance data between locations from a CSV file and adds bidirectional edges to the graph.
  * 
  * This function reads a CSV file containing distance data (driving and walking times) between locations and adds 
  * bidirectional edges to the graph, with the corresponding travel times.
  * 
  * @param filename The path to the CSV file containing distance data.
  * @return void
  * 
  * complexity O(M) where M is the number of distance entries in the CSV file.
  */
 void loadDistances(const std::string &filename) {
     std::ifstream file(filename);
     if (!file.is_open()) {
         std::cout << "Erro ao abrir ficheiro: " << filename << std::endl;
         return;
     }
 
     std::string line;
     getline(file, line); // Skip header
     while (getline(file, line)) {
         std::stringstream ss(line);
         std::string loc1, loc2, drivingStr, walkingStr;
         getline(ss, loc1, ',');
         getline(ss, loc2, ',');
         getline(ss, drivingStr, ',');
         getline(ss, walkingStr, ',');
 
         if (loc1.empty() || loc2.empty()) continue;
 
         double driveTime = (drivingStr == "X") ? -1 : stod(drivingStr);
         double walkTime = (walkingStr == "X") ? -1 : stod(walkingStr);
 
         cityGraph.addBidirectionalEdge(loc1, loc2, walkTime, driveTime);
     }
     file.close();
     std::cout << "Distâncias carregadas com sucesso!\n";
 }
 
 /**
  * @brief Displays the main menu to the user.
  * 
  * This function presents a menu with options for loading data, calculating routes, or exiting the program.
  * 
  * @return void
  */
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
 
 /**
  * @brief Calculates the best driving route between two locations using Dijkstra's algorithm.
  * 
  * This function prompts the user for an origin and destination, performs a Dijkstra search on the city graph to find 
  * the best driving route, and displays the resulting path and total driving time.
  * 
  * @return void
  * 
  * complexity O(V + E log V) where V is the number of vertices and E is the number of edges in the graph.
  */
 void calculateBestRoute() {
     std::string origem, destino;
     std::cout << "Código de origem: ";
     std::cin >> origem;
     std::cout << "Código de destino: ";
     std::cin >> destino;
 
     cityGraph.dijkstraDriving(origem);
     auto destVertex = cityGraph.findVertex(origem); // Use the same type to find vertices
 
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
 
 /**
  * @brief Placeholder function for unimplemented features.
  * 
  * This function is a placeholder for future features that have not yet been implemented.
  * 
  * @param featureName The name of the feature being called.
  * @return void
  */
 void placeholderFunction(const std::string &featureName) {
     std::cout << "Função '" << featureName << "' ainda não implementada." << std::endl;
 }
 
 /**
  * @brief Main entry point for the program.
  * 
  * This is the main driver function that presents the user with a menu, allows them to load data, calculate routes, 
  * and choose from various options until they choose to exit the program.
  * 
  * @return int Exit status of the program (0 for success).
  * 
  * complexity O(N) where N is the number of menu choices, executed until the user exits.
  */
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
 