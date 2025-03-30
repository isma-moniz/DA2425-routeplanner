#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <string>
#include "graph.hpp"

/**
 * @brief Structure to hold data for route calculation, including restrictions and other parameters.
 * 
 * This structure stores data related to a route calculation, including the source and destination nodes,
 * any nodes or segments to avoid, nodes to include, and max walking time for environmental routes.
 */
struct Data {
    std::string mode; ///< Mode of transportation, such as "driving" or "environmental".
    int source = -1; ///< Source node ID (initialized to -1 indicating no source).
    int destination = -1; ///< Destination node ID (initialized to -1 indicating no destination).
    std::vector<int> avoidNodes; ///< List of node IDs to avoid.
    std::vector<std::pair<int, int>> avoidSegments; ///< List of segment pairs (id1, id2) to avoid.
    int includeNode = -1; ///< Node ID to include in the route (default -1 means no node is forced).
    int maxWalkTime = -1; ///< Maximum walking time in minutes (for environmental routes).
};

/**
 * @brief Class for handling operations related to loading data, route calculations, and batch processing.
 * 
 * This class provides methods to load location and road data, calculate routes using Dijkstra's algorithm,
 * handle batch operations, and more.
 */
class StorageHandler {
public:
    /**
     * @brief Loads location data from a CSV file.
     * 
     * This function reads location data from the specified CSV file and loads it into the system.
     * 
     * @param locationsFile Path to the locations CSV file.
     * 
     * Time complexity: O(N), where N is the number of locations to load and parse from the file.
     */
    void loadLocations(const std::string& locationsFile);

    /**
     * @brief Loads road data from a CSV file.
     * 
     * This function reads road data (connections between locations) from the specified CSV file.
     * 
     * @param roadFile Path to the roads CSV file.
     * 
     * Time complexity: O(M), where M is the number of roads to load and parse from the file.
     */
    void loadRoads(const std::string& roadFile);

    /**
     * @brief Calls Dijkstra's algorithm to find the shortest path between two locations.
     * 
     * This function invokes Dijkstra's algorithm to find the best route between the given source and destination.
     * 
     * @param source The source location ID.
     * @param dest The destination location ID.
     * 
     * Time complexity: O(E log V), where E is the number of edges and V is the number of vertices in the graph.
     */
    void callDijkstra(const std::string& source, const std::string& dest);

    /**
     * @brief Calls Dijkstra's algorithm to find the shortest path with restrictions.
     * 
     * This function invokes a restricted version of Dijkstra's algorithm that takes into account the provided
     * nodes and segments to avoid and any required node to include.
     * 
     * @param src The source location ID.
     * @param dest The destination location ID.
     * @param avoidNodes A comma-separated string of node IDs to avoid.
     * @param avoidSegments A comma-separated string of road segments to avoid.
     * @param includeNode A string representing a node to include in the route.
     * 
     * Time complexity: O(E log V), where E is the number of edges and V is the number of vertices.
     * The complexity is similar to Dijkstra's algorithm with additional constraints.
     */
    void callRestrictedDijkstra(const std::string& src, const std::string& dest, 
        const std::string& avoidNodes, const std::string& avoidSegments, const std::string& includeNode);

    /**
     * @brief Calculates an environmentally friendly route that includes walking.
     * 
     * This function calculates a route combining both driving and walking, with the constraint of a maximum
     * walking time and any segments or nodes to avoid.
     * 
     * @param source The source location ID.
     * @param destination The destination location ID.
     * @param maxWalkingTime Maximum allowed walking time in minutes.
     * @param avoidNodes A list of node IDs to avoid.
     * @param avoidSegments A list of segment pairs (id1, id2) to avoid.
     * 
     * Time complexity: O(E log V), where E is the number of edges and V is the number of vertices in the graph.
     * Similar to the standard Dijkstra algorithm with additional considerations for walking paths.
     */
    void calculateEnvironmentalRoute(int source, int destination, int maxWalkingTime, std::vector<int> avoidNodes, std::vector<std::pair<int, int>> avoidSegments);

    /**
     * @brief Parses input data for batch processing from a file.
     * 
     * This function parses input from a file to populate the `Data` structure for batch processing.
     * 
     * @param data Pointer to a `Data` structure where parsed data will be stored.
     * 
     * @return int Returns 0 if successful, non-zero if an error occurred.
     * 
     * Time complexity: O(N), where N is the number of lines or data entries to parse.
     */
    int parseBatchInput(Data* data);

    /**
     * @brief Executes a batch function based on parsed data.
     * 
     * This function processes the batch data (from the `Data` structure) and executes the necessary route planning functions.
     * 
     * @param data The batch data structure containing the parsed input.
     * 
     * Time complexity: O(N), where N is the number of operations or routes to process in the batch.
     */
    void callBatchFunction(const Data& data);

private:
    Graph<int> cityGraph; ///< The graph structure representing the city with locations and roads.

    /**
     * @brief Helper function to parse a comma-separated list of integers.
     * 
     * @param str A comma-separated string of integers.
     * @return std::vector<int> A vector of integers parsed from the string.
     * 
     * Time complexity: O(K), where K is the number of integers in the string.
     */
    std::vector<int> parseCommaSeparatedIntegers(const std::string& str);

    /**
     * @brief Helper function to parse a comma-separated list of pairs (e.g., road segments).
     * 
     * @param str A comma-separated string of pairs in the form (id1,id2).
     * @return std::vector<std::pair<int, int>> A vector of pairs parsed from the string.
     * 
     * Time complexity: O(K), where K is the number of pairs in the string.
     */
    std::vector<std::pair<int, int>> parsePairs(const std::string& str);
};

#endif
