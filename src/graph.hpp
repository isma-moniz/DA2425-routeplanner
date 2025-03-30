#ifndef GRAPH_H
#define GRAPH_H

#include <algorithm>
#include <limits>
#include <queue>
#include <stdexcept>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <optional>
#include <fstream>

template <class T>
class Edge;

#define INF std::numeric_limits<double>::max()

/******************** Vertex ********************/


/**
 * @brief Vertex class representing a single node in the graph.
 * 
 * This class stores information about a vertex in the graph, including its ID, associated code, adjacency list, and several flags 
 * (visited, processing, parking). It also provides methods for adding and removing edges, setting attributes, and retrieving 
 * vertex-related information like its distance or indegree.
 */
template <class T>
class Vertex {
public:
    /**
     * @brief Constructs a Vertex with the given parameters.
     * 
     * @param in The ID or information of the vertex.
     * @param code The code associated with the vertex.
     * @param parking Indicates if this vertex has parking.
     * 
     * @note This function runs in O(1) time.
     */
    Vertex(T in, const std::string& code, bool parking);
    bool operator<(Vertex<T>& vertex) const; // comparison operator overload
    /**
     * @brief Compares two vertices based on their ID.
     * 
     * @return True if the current vertex's ID is smaller than the other vertex's ID.
     * 
     * @note This function runs in O(1) time.
     */
    
    T getInfo() const;
    const std::string& getCode() const;
    std::vector<Edge<T> *> getAdj() const;
    bool isVisited() const;
    bool isProcessing() const;
    bool hasParking() const;
    bool isAvailable() const;
    unsigned int getIndegree() const;
    double getDist() const;
    Edge<T>* getPath() const;
    std::vector<Edge<T>*> getIncoming() const;

    void setInfo(T info);
    void setCode(const std::string& code);
    void setAvailable(bool available);
    void setVisited(bool visited);
    void setProcessing(bool processing);
    void setParking(bool parking);

    int getLow() const;
    void setLow(int value);
    int getNum() const;
    void setNum(int value);

    void setIndegree(unsigned int indegree);
    void setDist(double dist);
    void setPath(Edge<T>* path);
    Edge<T>* addEdge(Vertex<T>* dest, double w, double d);
    /**
     * @brief Adds an edge from this vertex to the destination vertex.
     * 
     * @param dest The destination vertex.
     * @param w The weight of the driving edge.
     * @param d The weight of the walking edge.
     * 
     * @return A pointer to the newly created edge.
     * 
     * @note This function runs in O(1) time since we are adding to a vector.
     */
    bool removeEdge(T in);
    void removeOutgoingEdges();

    // friend class MutablePriorityQueue<Vertex>; -- probably not necessary yet
protected:
    T info; // we will store the id
    std::string code;
    std::vector<Edge<T> *> adj; // if we don't ever need to have the edges ordered, we will change this to unordered_set for constant time lookup, add and remove

    bool visited = false; // used by DFS, BFS, Primm
    bool processing = false; // used to detect cycles
    bool parkingSpace = false;
    bool available = true;
    
    int low = -1, num = -1; // used by SCC Tarjan
    unsigned int indegree; // used by topsort
    double dist = 0;
    Edge<T>* path = nullptr;

    std::vector<Edge<T>*> incoming;
    int queueIndex = 0; // for MutablePriorityQueue and UFDS
    
    void deleteEdge(Edge<T>* edge);
};

/******************** Edge ********************/

/**
 * @brief Edge class representing an edge between two vertices.
 * 
 * This class stores information about an edge in the graph, including the origin and destination vertices, 
 * the weights for driving and walking times, the flow on the edge, and the reverse edge for bidirectional links.
 */
template <class T>
class Edge {
public:
    /**
     * @brief Constructs an Edge with the given parameters.
     * 
     * @param orig The origin vertex.
     * @param dest The destination vertex.
     * @param w The driving time weight for the edge.
     * @param d The walking time weight for the edge.
     * 
     * @note This function runs in O(1) time.
     */
    Edge(Vertex<T>* orig, Vertex<T> *dest, double w, double d);

    Vertex<T>* getDest() const;
    double getDriveTime() const;
    double getWalkTime() const;
    bool isAvailable() const;
    Vertex<T>* getOrigin() const;
    Edge<T>* getReverse() const;
    double getFlow() const;

    void setAvailable(bool selected);
    void setReverse(Edge<T>* reverse);
    void setFlow(double flow);
protected:
    Vertex<T>* dest;
    double walkTime;
    double driveTime;

    bool available = true; // used for marking already used routes

    Vertex<T>* origin;
    Edge<T>* reverse = nullptr;

    double flow; // for flow related problems
};

/******************** Graph ********************/

/**
 * @brief Graph class representing a graph structure.
 * 
 * This class manages a collection of vertices and edges, supporting various graph algorithms such as Dijkstra's, 
 * and providing methods for adding/removing vertices and edges, as well as performing graph traversals and shortest path calculations.
 */
template <class T>
class Graph {
public:
    ~Graph();
    // Aux function to find a vertex with a given content.
    /**
     * @brief Finds a vertex in the graph by its ID.
     * 
     * @param in The ID of the vertex.
     * 
     * @return Pointer to the vertex with the given ID, or nullptr if not found.
     * 
     * @note This function runs in O(1) time, as it uses a hash map for constant time lookups.
     */
    Vertex<T>* findVertex(const T& in) const;
    // Aux function to find a vertex with a given code.
    /**
     * @brief Finds a vertex in the graph by its code.
     * 
     * @param code The code of the vertex.
     * 
     * @return Pointer to the vertex with the given code, or nullptr if not found.
     * 
     * @note This function runs in O(1) time, as it uses a hash map for constant time lookups.
     */
    Vertex<T>* findVertex(const std::string& code) const;
    // Add vertex with a given info to graph.
    /**
     * @brief Adds a vertex with the given ID and code to the graph.
     * 
     * @param in The ID of the vertex.
     * @param code The code associated with the vertex.
     * @param parking Indicates whether the vertex has parking.
     * 
     * @return True if the vertex was successfully added, false otherwise.
     * 
     * @note This function runs in O(1) time, as it only involves inserting into a hash map.
     */
    bool addVertex(const T& in, const std::string& code, bool parking);
    bool removeVertex(const T& in);
    // add edge to the graph given contents of source and dest as well as weights
    /**
     * @brief Adds an edge between two vertices, with specified driving and walking times.
     * 
     * @param source The source vertex ID.
     * @param dest The destination vertex ID.
     * @param w The driving time weight.
     * @param d The walking time weight.
     * 
     * @return True if the edge was successfully added, false otherwise.
     * 
     * @note This function runs in O(1) time, as it involves inserting the edge into the adjacency list.
     */
    bool addEdge(const T& source, const T& dest, double w, double d);
    bool addEdge(const std::string& source, const std::string& dest, double w, double d);
    bool removeEdge(const T& source, const T& dest);
    
    /**
     * @brief Adds a bidirectional edge between two vertices, with specified driving and walking times.
     * 
     * @param source The source vertex ID.
     * @param dest The destination vertex ID.
     * @param w The driving time weight.
     * @param d The walking time weight.
     * 
     * @return True if the bidirectional edge was successfully added, false otherwise.
     * 
     * @note This function runs in O(1) time as it adds two edges (one for each direction) to the graph.
     */
    bool addBidirectionalEdge(const T &source, const T& dest, double w, double d);
    bool addBidirectionalEdge(const std::string& source, const std::string& dest, double w, double d);

    /**
     * @brief Gets the number of vertices in the graph.
     * 
     * @return The number of vertices.
     * 
     * @note This function runs in O(1) time.
     */
    int getNumVertex() const;

    /**
     * @brief Retrieves all vertices in the graph.
     * 
     * @return A vector of pointers to all vertices.
     * 
     * @note This function runs in O(V) time, where V is the number of vertices in the graph.
     */
    std::vector<Vertex<T>*> getVertexSet() const;

    /**
     * @brief Calculates the fastest driving path with alternative routes.
     * 
     * @param origin The starting vertex.
     * @param destination The destination vertex.
     * 
     * @note This function may run in O((V + E) * log(V)) depending on the algorithm used.
     */
    void fastestDrivingPathWithAlt(const T& origin, const T& destination);

    /**
     * @brief Calculates the shortest path for driving using Dijkstra's algorithm.
     * 
     * @param origin The starting vertex.
     * @param destination The destination vertex.
     * 
     * @return A vector of edges representing the shortest driving path.
     * 
     * @note This function runs in O((V + E) * log(V)) where V is the number of vertices and E is the number of edges.
     */
    std::vector<Edge<T>*> dijkstraDriving(const T& origin, const T& destination);

    /**
     * @brief Calculates the shortest path for walking using Dijkstra's algorithm.
     * 
     * @param origin The starting vertex.
     * @param destination The destination vertex.
     * 
     * @return A vector of edges representing the shortest walking path.
     * 
     * @note This function runs in O((V + E) * log(V)) where V is the number of vertices and E is the number of edges.
     */
    std::vector<Edge<T>*> dijkstraWalking(const T& origin, const T& destination);

    /**
     * @brief Retrieves all vertices in the graph that have parking.
     * 
     * @return A vector of vertices with parking.
     * 
     * @note This function runs in O(V) time.
     */
    std::vector<Vertex<T>*> getAllParkingVertices() const;

    /**
     * @brief Calculates the fastest restricted driving path with conditions.
     * 
     * @param origin The starting vertex.
     * @param destination The destination vertex.
     * @param avoidNodes A list of nodes to avoid.
     * @param avoidSegments A list of segments to avoid.
     * @param stop An optional vertex to stop at.
     * 
     * @note This function may run in O((V + E) * log(V)) depending on the algorithm used.
     */
    void fastestRestrictedDrivingPath(const T& origin, const T& destination, std::vector<T> avoidNodes, 
        std::vector<std::pair<T,T>> avoidSegments, std::optional<T> stop);

protected:
    std::unordered_map<int, Vertex<T>*> idToVertexMap; // replaced vertexSet with this to provide constant time lookup by id for internal graph operations 
    std::unordered_map<std::string, Vertex<T>*> codeToVertexMap; // provides constant time lookup by vertex code
    
    // matrixes for Floyd-Warshall
    double** distMatrix = nullptr;
    int** pathMatrix = nullptr;

    // int findVertexIdx(const T& in) const; no longer needed
};

void deleteMatrix(int** m, int n);
void deleteMatrix(double** m, int n);

////////////////////////////////////////////////////////////////////////
// ---------------------Implementation starts here--------------------//
////////////////////////////////////////////////////////////////////////

/************************* Vertex  **************************/

/**
 * @brief Constructs a Vertex with the given parameters.
 * 
 * This constructor initializes a vertex with the given ID, code, and parking status.
 * 
 * @param in The ID or information of the vertex.
 * @param code The code associated with the vertex.
 * @param parking Indicates if this vertex has parking.
 * 
 * @note This function runs in O(1) time, as it only initializes member variables.
 */
template <class T>
Vertex<T>::Vertex(T in, const std::string& code, bool parking): info(in), code(code), parkingSpace(parking) {}

/**
 * @brief Adds an edge from this vertex to the destination vertex.
 * 
 * This method creates a new edge between this vertex (the origin) and the destination vertex.
 * The edge has specified walking and driving time weights. The edge is added to the adjacency 
 * list of the current vertex and the incoming edge list of the destination vertex.
 * 
 * @param dest The destination vertex for the new edge.
 * @param w The walking time weight for the edge.
 * @param d The driving time weight for the edge.
 * 
 * @return A pointer to the newly created edge.
 * 
 * @note This function runs in O(1) time, as it involves pushing an edge onto the adjacency and incoming lists (both of which are vectors).
 */
template <class T>
Edge<T> * Vertex<T>::addEdge(Vertex<T> *dest, double w, double d) {
    auto newEdge = new Edge<T>(this, dest, w, d);
    adj.push_back(newEdge);
    dest->incoming.push_back(newEdge);
    return newEdge;
}

/**
 * @brief Removes an outgoing edge from this vertex to the specified destination vertex.
 * 
 * This method searches for an outgoing edge with the given destination vertex (specified by the input parameter `in`),
 * and removes it if found. It deletes the edge and updates the adjacency list of the current vertex.
 * If the edge is successfully removed, the method returns true. If the edge does not exist, it returns false.
 * 
 * @param in The ID of the destination vertex of the edge to be removed.
 * 
 * @return `true` if the edge was successfully removed, `false` if no such edge was found.
 * 
 * @note This function runs in O(E) time in the worst case, where E is the number of outgoing edges of the vertex. 
 *       The complexity arises from the need to search through the adjacency list to find the edge to remove.
 */
template <class T>
bool Vertex<T>::removeEdge(T in) {
    bool removedEdge = false;
    auto it = adj.begin();
    while (it != adj.end()) {
        Edge<T> *edge = *it;
        Vertex<T> *dest = edge->getDest();
        if (dest->getInfo() == in) {
            it = adj.erase(it);
            deleteEdge(edge);
            removedEdge = true; // allows for multiple edges to connect the same pair of vertices (multigraph)
        }
        else {
            it++;
        }
    }
    return removedEdge;
}

/**
 * @brief Removes all outgoing edges from this vertex.
 * 
 * This method iterates over the vertex's adjacency list and removes all outgoing edges. 
 * It deletes the edges and clears the adjacency list for the current vertex.
 * 
 * @note This function runs in O(E) time, where E is the number of outgoing edges of the vertex, as each edge must be 
 *       removed and deleted individually.
 */
template <class T>
void Vertex<T>::removeOutgoingEdges() {
    auto it = adj.begin();
    while (it != adj.end()) {
        Edge<T> *edge = *it;
        it = adj.erase(it);
        deleteEdge(edge);
    }
}

/**
 * @brief Compares two vertices based on their distance.
 * 
 * This operator overloads the less-than operator (`<`) to allow for comparison between two vertices. 
 * The comparison is based on the `dist` attribute of the vertex, which is often used for sorting or priority queue operations 
 * in algorithms like Dijkstra's.
 * 
 * @param vertex The vertex to compare this vertex against.
 * 
 * @return `true` if the `dist` of this vertex is less than the `dist` of the other vertex, `false` otherwise.
 * 
 * @note The time complexity of this function is O(1), as it only compares the `dist` values of two vertices.
 */
template <class T>
bool Vertex<T>::operator<(Vertex<T>& vertex) const {
    return this->dist < vertex.dist;
}

/**
 * @brief Returns the info (ID) of the vertex.
 * 
 * This function retrieves the `info` attribute of the vertex, which is typically used to store the unique ID or identifier 
 * for the vertex.
 * 
 * @return The `info` of the vertex.
 * 
 * @note The time complexity of this function is O(1), as it simply returns the `info` attribute.
 */
template <class T>
T Vertex<T>::getInfo() const {
    return this->info;
}

/**
 * @brief Returns the code (a string) associated with the vertex.
 * 
 * This function retrieves the `code` attribute of the vertex, which can be used for additional labeling or identification.
 * 
 * @return The `code` string associated with the vertex.
 * 
 * @note The time complexity of this function is O(1), as it simply returns the `code` attribute.
 */
template <class T>
const std::string& Vertex<T>::getCode() const {
    return this->code;
}

/**
 * @brief Sets the code (a string) for the vertex.
 * 
 * This function updates the `code` attribute of the vertex, typically used for assigning or changing the vertex's label or identifier.
 * 
 * @param code The new code to assign to the vertex.
 * 
 * @note The time complexity of this function is O(1), as it simply sets the `code` attribute.
 */
template <class T>
void Vertex<T>::setCode(const std::string& code) {
    this->code = code;
}

/**
 * @brief Sets the availability status of the vertex.
 * 
 * This function updates the `available` attribute of the vertex, 
 * which determines whether the vertex can be used in certain graph operations.
 * 
 * @param available The new availability status (`true` for available, `false` for unavailable).
 * 
 * @note The time complexity of this function is O(1), as it simply assigns a value to an attribute.
 */
template <class T>
void Vertex<T>::setAvailable(bool available) {
    this->available = available;
}

/**
 * @brief Checks if the vertex is available.
 * 
 * This function returns the current value of the `available` attribute.
 * 
 * @return `true` if the vertex is available, `false` otherwise.
 * 
 * @note The time complexity of this function is O(1), as it simply returns the value of an attribute.
 */
template <class T>
bool Vertex<T>::isAvailable() const{
    return this->available;
}

/**
 * @brief Retrieves the `low` value of the vertex.
 * 
 * The `low` value is used in Tarjan’s algorithm for finding strongly connected components (SCCs).
 * 
 * @return The `low` value of the vertex.
 * 
 * @note The time complexity of this function is O(1), as it simply returns the value of an attribute.
 */
template <class T>
int Vertex<T>::getLow() const {
    return this->low;
}

/**
 * @brief Sets the `low` value of the vertex.
 * 
 * The `low` value is used in Tarjan’s algorithm for finding strongly connected components (SCCs).
 * 
 * @param value The new `low` value.
 * 
 * @note The time complexity of this function is O(1), as it simply assigns a value to an attribute.
 */
template <class T>
void Vertex<T>::setLow(int value) {
    this->low = value;
}

/**
 * @brief Retrieves the `num` value of the vertex.
 * 
 * The `num` value is used in Tarjan’s algorithm to represent the discovery time of the vertex.
 * 
 * @return The `num` value of the vertex.
 * 
 * @note The time complexity of this function is O(1), as it simply returns the value of an attribute.
 */
template <class T>
int Vertex<T>::getNum() const {
    return this->num;
}

/**
 * @brief Sets the `num` value of the vertex.
 * 
 * The `num` value is used in Tarjan’s algorithm to represent the discovery time of the vertex.
 * 
 * @param value The new `num` value.
 * 
 * @note The time complexity of this function is O(1), as it simply assigns a value to an attribute.
 */
template <class T>
void Vertex<T>::setNum(int value) {
    this->num = value;
}

/**
 * @brief Retrieves the adjacency list of the vertex.
 * 
 * This function returns a vector of pointers to the edges that originate from this vertex.
 * 
 * @return A vector of pointers to the outgoing edges of the vertex.
 * 
 * @note The time complexity of this function is O(1), as it simply returns a reference to the adjacency list.
 */
template <class T>
std::vector<Edge<T>*> Vertex<T>::getAdj() const {
    return this->adj;
}

/**
 * @brief Checks if the vertex has been visited.
 * 
 * This function is primarily used in graph traversal algorithms such as DFS and BFS.
 * 
 * @return `true` if the vertex has been visited, `false` otherwise.
 * 
 * @note The time complexity of this function is O(1), as it simply returns the value of an attribute.
 */
template <class T>
bool Vertex<T>::isVisited() const {
    return this->visited;
}

/**
 * @brief Checks if the vertex is currently being processed.
 * 
 * This function is useful in cycle detection algorithms to track if a node is in the recursion stack.
 * 
 * @return `true` if the vertex is in processing, `false` otherwise.
 * 
 * @note The time complexity of this function is O(1), as it simply returns the value of an attribute.
 */
template <class T>
bool Vertex<T>::isProcessing() const {
    return this->processing;
}

/**
 * @brief Checks if the vertex has a parking space.
 * 
 * This function is used in applications where parking locations need to be identified within the graph.
 * 
 * @return `true` if the vertex has a parking space, `false` otherwise.
 * 
 * @note The time complexity of this function is O(1), as it simply returns the value of an attribute.
 */
template <class T>
bool Vertex<T>::hasParking() const {
    return this->parkingSpace;
}

/**
 * @brief Gets the in-degree of the vertex.
 * 
 * The in-degree represents the number of incoming edges to this vertex.
 * 
 * @return The number of incoming edges to this vertex.
 * 
 * @note The time complexity of this function is O(1), as it simply returns the value of an attribute.
 */
template <class T>
unsigned int Vertex<T>::getIndegree() const {
    return this->indegree;
}

/**
 * @brief Gets the distance value associated with this vertex.
 * 
 * This value is typically used in shortest path algorithms such as Dijkstra’s algorithm.
 * 
 * @return The distance value of the vertex.
 * 
 * @note The time complexity of this function is O(1), as it simply returns the value of an attribute.
 */
template <class T>
double Vertex<T>::getDist() const {
    return this->dist;
}


/**
 * @brief Gets the path to this vertex.
 * 
 * This function returns the edge that leads to this vertex in a shortest path tree.
 * 
 * @return A pointer to the edge representing the path to this vertex.
 * 
 * @note The time complexity of this function is O(1), as it simply returns a pointer.
 */
template <class T>
Edge<T> *Vertex<T>::getPath() const {
    return this->path;
}

/**
 * @brief Gets all incoming edges to this vertex.
 * 
 * This function returns a vector of pointers to all edges that point to this vertex.
 * 
 * @return A vector of pointers to incoming edges.
 * 
 * @note The time complexity of this function is O(1) for returning the reference to the vector.
 *       However, iterating over the returned vector would take O(k), where k is the number of incoming edges.
 */
template <class T>
std::vector<Edge<T> *> Vertex<T>::getIncoming() const {
    return this->incoming;
}

/**
 * @brief Sets the information stored in the vertex.
 * 
 * This function updates the `info` attribute of the vertex.
 * 
 * @param in The new information to be stored in the vertex.
 * 
 * @note The time complexity of this function is O(1), as it simply assigns a value to an attribute.
 */
template <class T>
void Vertex<T>::setInfo(T in) {
    this->info = in;
}

/**
 * @brief Sets the visited status of the vertex.
 * 
 * This function updates whether the vertex has been visited, typically used in graph traversal algorithms.
 * 
 * @param visited A boolean indicating whether the vertex has been visited.
 * 
 * @note The time complexity of this function is O(1), as it involves a simple assignment.
 */
template <class T>
void Vertex<T>::setVisited(bool visited) {
    this->visited = visited;
}

/**
 * @brief Sets the processing status of the vertex.
 * 
 * This function updates whether the vertex is currently being processed, often used in algorithms like 
 * depth-first search (DFS) for cycle detection.
 * 
 * @param processing A boolean indicating whether the vertex is being processed.
 * 
 * @note The time complexity of this function is O(1), as it involves a simple assignment.
 */
template <class T>
void Vertex<T>::setProcessing(bool processing) {
    this->processing = processing;
}

/**
 * @brief Sets whether the vertex has a parking space.
 * 
 * This function updates the `parkingSpace` attribute of the vertex.
 * 
 * @param parking A boolean indicating whether the vertex has parking available.
 * 
 * @note The time complexity of this function is O(1), as it involves a simple assignment.
 */
template <class T>
void Vertex<T>::setParking(bool parking) {
    this->parkingSpace = parking;
}

/**
 * @brief Sets the indegree of the vertex.
 * 
 * The indegree represents the number of incoming edges to this vertex.
 * 
 * @param indegree The new indegree value to be set.
 * 
 * @note The time complexity of this function is O(1), as it involves a simple assignment operation.
 */
template <class T>
void Vertex<T>::setIndegree(unsigned int indegree) {
    this->indegree = indegree;
}

/**
 * @brief Sets the distance value associated with the vertex.
 * 
 * This function is typically used in shortest-path algorithms (e.g., Dijkstra's algorithm) 
 * to store the computed distance from a source vertex.
 * 
 * @param dist The new distance value to be set.
 * 
 * @note The time complexity of this function is O(1), as it involves a simple assignment operation.
 */
template <class T>
void Vertex<T>::setDist(double dist) {
    this->dist = dist;
}

/**
 * @brief Sets the path (previous edge) leading to this vertex.
 * 
 * This function is used in pathfinding algorithms to keep track of the previous edge 
 * in a shortest or optimal path computation.
 * 
 * @param path A pointer to the edge that represents the previous step in the path.
 * 
 * @note The time complexity of this function is O(1), as it involves a simple pointer assignment.
 */
template <class T>
void Vertex<T>::setPath(Edge<T> *path) {
    this->path = path;
}

/**
 * @brief Deletes an edge and removes its reference from the destination vertex's incoming edge list.
 * 
 * This function removes the given edge from the `incoming` list of the destination vertex
 * and then deallocates the edge memory.
 * 
 * @param edge A pointer to the edge to be deleted.
 * 
 * @note The function iterates through the destination vertex's `incoming` edges to find and remove the correct edge.
 *       The worst-case time complexity is **O(E_in)**, where **E_in** is the number of incoming edges to the destination vertex.
 */
template <class T>
void Vertex<T>::deleteEdge(Edge<T> *edge) {
    Vertex<T> *dest = edge->getDest();
    // Remove the corresponding edge from the incoming list
    auto it = dest->incoming.begin();
    while (it != dest->incoming.end()) {
        if ((*it)->getOrig()->getInfo() == info) {
            it = dest->incoming.erase(it);
        }
        else {
            it++;
        }
    }
    delete edge;
}

/********************** Edge  ****************************/

/**
 * @brief Constructs an Edge object with the given origin, destination, walk time, and drive time.
 * 
 * @param orig Pointer to the origin vertex.
 * @param dest Pointer to the destination vertex.
 * @param w Walking time associated with the edge.
 * @param d Driving time associated with the edge.
 * 
 * @note The constructor simply initializes the member variables, so its time complexity is **O(1)**.
 */
template <class T>
Edge<T>::Edge(Vertex<T> *orig, Vertex<T> *dest, double w, double d): origin(orig), dest(dest), walkTime(w), driveTime(d) {}

/**
 * @brief Gets the destination vertex of the edge.
 * 
 * @return Pointer to the destination vertex.
 * 
 * @note Accessing a member variable is a constant-time operation, so the time complexity is **O(1)**.
 */
template <class T>
Vertex<T> * Edge<T>::getDest() const {
    return this->dest;
}

/**
 * @brief Gets the driving time associated with the edge.
 * 
 * @return The drive time as a double.
 * 
 * @note Accessing a member variable is a constant-time operation, so the time complexity is **O(1)**.
 */
template <class T>
double Edge<T>::getDriveTime() const {
    return this->driveTime;
}

/**
 * @brief Gets the walking time associated with the edge.
 * 
 * @return The walk time as a double.
 * 
 * @note Accessing a member variable is a constant-time operation, so the time complexity is **O(1)**.
 */
template <class T>
double Edge<T>::getWalkTime() const {
    return this->walkTime;
}

/**
 * @brief Gets the origin vertex of the edge.
 * 
 * @return Pointer to the origin vertex.
 * 
 * @note Accessing a member variable is a constant-time operation, so the time complexity is **O(1)**.
 */
template <class T>
Vertex<T> * Edge<T>::getOrigin() const {
    return this->origin;
}

/**
 * @brief Gets the reverse edge associated with this edge.
 * 
 * @return Pointer to the reverse edge if it exists, otherwise nullptr.
 * 
 * @note Accessing a member variable is a constant-time operation, so the time complexity is **O(1)**.
 */
template <class T>
Edge<T> *Edge<T>::getReverse() const {
    return this->reverse;
}

/**
 * @brief Checks if the edge is available.
 * 
 * @return True if the edge is available, false otherwise.
 * 
 * @note Accessing a boolean member variable is a constant-time operation, so the time complexity is **O(1)**.
 */
template <class T>
bool Edge<T>::isAvailable() const {
    return this->available;
}

/**
 * @brief Gets the current flow value of the edge.
 * 
 * @return The flow value as a double.
 * 
 * @note Accessing a member variable is a constant-time operation, so the time complexity is **O(1)**.
 */
template <class T>
double Edge<T>::getFlow() const {
    return flow;
}

/**
 * @brief Sets the availability status of the edge.
 * 
 * @param available Boolean value indicating whether the edge is available.
 * 
 * @note Modifying a boolean member variable is a constant-time operation, so the time complexity is **O(1)**.
 */
template <class T>
void Edge<T>::setAvailable(bool available) {
    this->available = available;
}

/**
 * @brief Sets the reverse edge associated with this edge.
 * 
 * @param reverse Pointer to the reverse edge.
 * 
 * @note Assigning a pointer to a member variable is a constant-time operation, so the time complexity is **O(1)**.
 */
template <class T>
void Edge<T>::setReverse(Edge<T> *reverse) {
    this->reverse = reverse;
}

/**
 * @brief Sets the flow value of the edge.
 * 
 * @param flow The new flow value as a double.
 * 
 * @note Assigning a double to a member variable is a constant-time operation, so the time complexity is **O(1)**.
 */
template <class T>
void Edge<T>::setFlow(double flow) {
    this->flow = flow;
}

/********************** Graph  ****************************/

/**
 * @brief Gets the number of vertices in the graph.
 * 
 * @return The total number of vertices in the graph.
 * 
 * @note Since `idToVertexMap.size()` is a constant-time operation, the time complexity is **O(1)**.
 */
template <class T>
int Graph<T>::getNumVertex() const {
    return idToVertexMap.size(); 
}

/**
 * @brief Retrieves a vector containing all the vertices in the graph.
 * 
 * @return A vector of pointers to all the vertices in the graph.
 * 
 * @note Iterating through all elements in `idToVertexMap` and inserting them into a vector takes **O(V)** time,
 *       where V is the number of vertices in the graph.
 */
template <class T>
std::vector<Vertex<T> *> Graph<T>::getVertexSet() const {
    std::vector<Vertex<T>*> vertices;
    for (const auto& pair : idToVertexMap) {
        vertices.push_back(pair.second);
    }
    return vertices;
}

/**
 * @brief Finds a vertex in the graph given its identifier.
 * 
 * @param in The identifier of the vertex.
 * @return A pointer to the vertex if found, otherwise `nullptr`.
 * 
 * @note The `find` operation on an unordered map runs in **O(1)** average time complexity,
 *       but in the worst case (highly unlikely due to hashing), it can be **O(V)**.
 */
template <class T>
Vertex<T>* Graph<T>::findVertex(const T &in) const {
    auto it = idToVertexMap.find(in);
    return (it != idToVertexMap.end()) ? it->second : nullptr;
}

/**
 * @brief Finds a vertex in the graph given its unique code.
 * 
 * @param code The unique code of the vertex.
 * @return A pointer to the vertex if found, otherwise `nullptr`.
 * 
 * @note The `find` operation on an unordered map runs in **O(1)** average time complexity,
 *       but in the worst case (due to hash collisions), it can be **O(V)**.
 */
template <class T>
Vertex<T>* Graph<T>::findVertex(const std::string& code) const {
    auto it = codeToVertexMap.find(code);
    return (it != codeToVertexMap.end()) ? it->second : nullptr;
}

/**
 * @brief Adds a vertex with a given identifier, unique code, and parking information to the graph.
 * 
 * @param in The unique identifier of the vertex.
 * @param code The unique code of the vertex.
 * @param parking Boolean indicating whether the vertex has parking.
 * @return `true` if the vertex was successfully added, `false` if a vertex with the given identifier already exists.
 * 
 * @note - Checking for existence in `idToVertexMap` runs in **O(1)** average time complexity.
 *       - Insertion into both `idToVertexMap` and `codeToVertexMap` also runs in **O(1)** on average.
 *       - Worst case complexity (due to hash collisions) is **O(V)**.
 *       - Overall, the expected time complexity is **O(1)**.
 */
template <class T>
bool Graph<T>::addVertex(const T &in, const std::string& code, bool parking) {
    if (idToVertexMap.find(in) != idToVertexMap.end()) return false;

    Vertex<T>* v = new Vertex<T>(in, code, parking);
    idToVertexMap[in] = v;
    codeToVertexMap[code] = v;
    return true;
}

/**
 * @brief Removes a vertex with a given content from the graph, along with all its edges.
 * 
 * @param in The unique identifier of the vertex to be removed.
 * @return `true` if the vertex was successfully removed, `false` if the vertex does not exist.
 * 
 * @details
 * - First, the function checks for the vertex in **O(1)** average time (unordered_map lookup).
 * - `removeOutGoingEdges()` iterates over the adjacency list and removes all outgoing edges. This takes **O(E')**, where **E'** is the number of edges originating from the vertex.
 * - The loop over `getIncoming()` removes each incoming edge from the origin vertices. Since each removal operation is **O(E'')**, where **E''** is the number of incoming edges, this loop runs in **O(E'')**.
 * - Erasing the vertex from the maps is **O(1)**.
 * - Overall, the complexity is **O(E') + O(E'') ≈ O(E)** in the worst case.
 */
template <class T>
bool Graph<T>::removeVertex(const T &in) {
    auto it = idToVertexMap.find(in);
    if (it == idToVertexMap.end()) return false; // vertex does not exist

    Vertex<T>* v = it->second;
    v->removeOutGoingEdges();
    for (Edge<T>* e : v->getIncoming()) {
        e->getOrigin()->removeEdge(in); // this is better than iterating through the whole vertex set, but still O(e)
    }
    idToVertexMap.erase(it);
    codeToVertexMap.erase(v->getCode());
    delete v; // free memory
    return true;
} 

/**
 * @brief Adds an edge to the graph between two existing vertices.
 * 
 * @param sourc The identifier of the source vertex.
 * @param dest The identifier of the destination vertex.
 * @param w The weight (walking time) of the edge.
 * @param d The driving time of the edge.
 * @return `true` if the edge was successfully added, `false` if either vertex does not exist.
 * 
 * @details
 * - Finding both source and destination vertices takes **O(1)** on average.
 * - Adding an edge to the adjacency list of the source vertex takes **O(1)**.
 * - Overall, the expected time complexity is **O(1)**.
 */
template <class T>
bool Graph<T>::addEdge(const T &sourc, const T &dest, double w, double d) {
    auto v1 = findVertex(sourc);
    auto v2 = findVertex(dest);
    if (v1 == nullptr || v2 == nullptr)
        return false;
    v1->addEdge(v2, w, d);
    return true;
}

/**
 * @brief Adds a directed edge to the graph between two existing vertices.
 * 
 * @param sourc The unique identifier of the source vertex.
 * @param dest The unique identifier of the destination vertex.
 * @param w The weight (walking time) of the edge.
 * @param d The driving time of the edge.
 * @return `true` if the edge was successfully added, `false` if either vertex does not exist.
 * 
 * @details
 * - Finding both source and destination vertices takes **O(1)** on average.
 * - Adding an edge to the adjacency list of the source vertex takes **O(1)**.
 * - Overall, the expected time complexity is **O(1)**.
 */
template <class T>
bool Graph<T>::addEdge(const std::string &sourc, const std::string &dest, double w, double d) {
    auto v1 = findVertex(sourc);
    auto v2 = findVertex(dest);
    if (v1 == nullptr || v2 == nullptr)
        return false;
    v1->addEdge(v2, w, d);
    return true;
}


/**
 * @brief Removes a directed edge from the graph.
 * 
 * @param sourc The unique identifier of the source vertex.
 * @param dest The unique identifier of the destination vertex.
 * @return `true` if the edge was successfully removed, `false` if either the source vertex does not exist or the edge does not exist.
 * 
 * @details
 * - Finding the source vertex takes **O(1)** on average.
 * - Removing the edge from the adjacency list of the source vertex takes **O(E')**, where **E'** is the number of edges originating from `sourc`.
 * - Overall, the worst-case time complexity is **O(E')**.
 */
template <class T>
bool Graph<T>::removeEdge(const T &sourc, const T &dest) {
    Vertex<T> * srcVertex = findVertex(sourc);
    if (srcVertex == nullptr) {
        return false;
    }
    return srcVertex->removeEdge(dest);
}

/**
 * @brief Adds a bidirectional (undirected) edge between two vertices.
 * 
 * @param sourc The unique identifier of the first vertex.
 * @param dest The unique identifier of the second vertex.
 * @param w The weight (walking time) of the edge.
 * @param d The driving time of the edge.
 * @return `true` if the edge was successfully added, `false` if either vertex does not exist.
 * 
 * @details
 * - Finding both vertices takes **O(1)** on average.
 * - Adding an edge to both adjacency lists takes **O(1)**.
 * - Setting the reverse edge pointers takes **O(1)**.
 * - Overall, the expected time complexity is **O(1)**.
 */
template <class T>
bool Graph<T>::addBidirectionalEdge(const T &sourc, const T &dest, double w, double d) {
    auto v1 = findVertex(sourc);
    auto v2 = findVertex(dest);
    if (v1 == nullptr || v2 == nullptr)
        return false;
    auto e1 = v1->addEdge(v2, w, d);
    auto e2 = v2->addEdge(v1, w, d);
    e1->setReverse(e2);
    e2->setReverse(e1);
    return true;
}

/**
 * @brief Adds a bidirectional (undirected) edge between two vertices, identified by their string identifiers.
 * 
 * @param sourc The unique identifier (string) of the first vertex.
 * @param dest The unique identifier (string) of the second vertex.
 * @param w The weight (walking time) of the edge.
 * @param d The driving time of the edge.
 * @return `true` if the edge was successfully added, `false` if either vertex does not exist.
 * 
 * @details
 * - Finding both vertices takes **O(1)** on average.
 * - Adding an edge to both adjacency lists takes **O(1)**.
 * - Setting the reverse edge pointers takes **O(1)**.
 * - Overall, the expected time complexity is **O(1)**.
 */
template <class T>
bool Graph<T>::addBidirectionalEdge(const std::string &sourc, const std::string &dest, double w, double d) {
    auto v1 = findVertex(sourc);
    auto v2 = findVertex(dest);
    if (v1 == nullptr || v2 == nullptr)
        return false;
    auto e1 = v1->addEdge(v2, w, d);
    auto e2 = v2->addEdge(v1, w, d);
    e1->setReverse(e2);
    e2->setReverse(e1);
    return true;
}


/**
 * @brief Deallocates a dynamically allocated integer matrix.
 * 
 * @param m Pointer to the matrix.
 * @param n Number of rows in the matrix.
 * 
 * @details
 * - Iterates over `n` rows, deleting each row (if not nullptr).
 * - Finally, deletes the array of row pointers.
 * - The overall time complexity is **O(n)**.
 */
inline void deleteMatrix(int **m, int n) {
    if (m != nullptr) {
        for (int i = 0; i < n; i++)
            if (m[i] != nullptr)
                delete [] m[i];
        delete [] m;
    }
}

/**
 * @brief Deallocates a dynamically allocated double matrix.
 * 
 * @param m Pointer to the matrix.
 * @param n Number of rows in the matrix.
 * 
 * @details
 * - Iterates over `n` rows, deleting each row (if not nullptr).
 * - Finally, deletes the array of row pointers.
 * - The overall time complexity is **O(n)**.
 */
inline void deleteMatrix(double **m, int n) {
    if (m != nullptr) {
        for (int i = 0; i < n; i++)
            if (m[i] != nullptr)
                delete [] m[i];
        delete [] m;
    }
}

/**
 * @brief Destructor for the Graph class.
 * 
 * @details
 * - Frees memory allocated for the distance and path matrices.
 * - The number of rows in each matrix is equal to the number of vertices.
 * - Each row contains an array of pointers, leading to a total complexity of **O(V)** for deallocation.
 */
template <class T>
Graph<T>::~Graph() {
    deleteMatrix(distMatrix, idToVertexMap.size());
    deleteMatrix(pathMatrix, idToVertexMap.size());
}

/**
 * @brief Comparator for ordering vertices in a priority queue based on distance.
 * 
 * @details
 * - Compares the `dist` values of two vertices.
 * - Designed for use in algorithms like Dijkstra’s, where a min-heap is required.
 * - The comparison operation itself is **O(1)**.
 */
template <class T>
struct vertexComp {
    bool operator()(Vertex<T>* a, Vertex<T>* b) {
        return a->getDist() > b->getDist(); // order for min-heap
    }
};

/**
 * @brief Finds the fastest driving path and an alternative path using Dijkstra’s algorithm.
 * 
 * @param origin The starting point of the route.
 * @param destination The endpoint of the route.
 * 
 * @details
 * - Uses **Dijkstra’s algorithm** to find the shortest driving path from `origin` to `destination`. 
 * - Stores the best route in `usedRoads`.
 * - If a valid path exists:
 *   - Marks edges as unavailable.
 *   - Runs Dijkstra again to find an **alternative route** (if possible).
 *   - Restores edges after computation.
 * - Outputs results to both **console** and **file**.
 * 
 * @complexity
 * - **Dijkstra’s Algorithm Complexity:** O((V + E) log V) for each run.
 * - **Setting Edge Availability:** O(E) (each edge in the best path is updated).
 * - **Total Complexity:** O((V + E) log V).
 */
template <class T>
void Graph<T>::fastestDrivingPathWithAlt(const T& origin, const T& destination) {
    std::ofstream file("../output.txt");

    std::vector<Edge<T>*> usedRoads = dijkstraDriving(origin, destination);

    std::cout << "Source: " << origin << "\n";
    std::cout << "Destination: " << destination << "\n";

    file << "Source: " << origin << "\n";
    file << "Destination: " << destination << "\n";
    
    if (usedRoads.empty()) {
        std::cout << "BestDrivingRoute:none\n";
        std::cout << "AlternativeDrivingRoute:none\n";
        file.close();
        return;
    }

    std::cout << "BestDrivingRoute: ";
    file << "BestDrivingRoute: ";
    for (auto edge : usedRoads) {std::cout << edge->getOrigin()->getInfo() << ","; file << edge->getOrigin()->getInfo() << ","; }
    std::cout << destination << "(" << usedRoads.back()->getDest()->getDist() << ")\n";
    file << destination << "(" << usedRoads.back()->getDest()->getDist() << ")\n";

    for (auto edge : usedRoads) {
        edge->setAvailable(false);
    }

    std::vector<Edge<T>*> altRoads = dijkstraDriving(origin, destination);
    if (!altRoads.empty()) {
        std::cout << "AlternativeDrivingRoute:";
        file << "AlternativeDrivingRoute:";
        for (auto edge : altRoads) {std::cout << edge->getOrigin()->getInfo() << ","; file << edge->getOrigin()->getInfo() << ",";}
        std::cout << destination << "(" << altRoads.back()->getDest()->getDist() << ")\n";
        file << destination << "(" << altRoads.back()->getDest()->getDist() << ")\n";
    } else {
        std::cout << "AlternativeDrivingRoute:none\n";
        file << "AlternativeDrivingRoute:none\n";
    }
    

    for (auto edge : usedRoads) {
        edge->setAvailable(true);
    }
    file.close();
    return;
}

/**
 * @brief Computes the shortest driving path from `origin` to `destination` using Dijkstra’s Algorithm.
 * 
 * @param origin The starting vertex.
 * @param destination The target vertex.
 * @return std::vector<Edge<T>*> The list of edges representing the shortest path.
 * 
 * @details
 * - Initializes all vertex distances to **infinity (INF)**.
 * - Uses a **min-heap (priority queue)** for efficient extraction of the next closest vertex.
 * - Updates the shortest paths dynamically as it explores neighbors.
 * - Returns the **sequence of edges** forming the optimal path from `origin` to `destination`.
 * - If no path is found, returns an empty vector.
 * 
 * @throws std::runtime_error if `origin` or `destination` do not exist.
 * 
 * @complexity
 * - **Priority Queue Operations:** O(log V) per insertion/extraction.
 * - **Total Complexity:** O((V + E) log V).
 */
template <class T>
std::vector<Edge<T>*> Graph<T>::dijkstraDriving(const T& origin, const T& destination) {
    std::priority_queue<Vertex<T>*, std::vector<Vertex<T>*>, vertexComp<T>> pq; // min-heap

    // initialization
    for (auto& it : this->idToVertexMap) {
        it.second->setDist(INF);
        it.second->setPath(nullptr);
        it.second->setVisited(false);
    }
    
    // lookup source and dest vertices
    auto it = idToVertexMap.find(origin);
    if (it == idToVertexMap.end()) {
        throw std::runtime_error("Dijkstra error: could not find vertex with id " + std::to_string(origin) + "\n");
    }
    Vertex<T>* originVert = it->second;

    auto it2 = idToVertexMap.find(destination);
    if (it2 == idToVertexMap.end()) {
        throw std::runtime_error("Dijkstra error: could not find vertex with id " + std::to_string(destination) + "\n");
    }
    Vertex<T>* destVert = it2->second;

    // pq initialization
    originVert->setDist(0);
    pq.push(originVert);
    // std::cout << "Starting Dijkstra from " << origin << " to " << destination << "\n";

    while (!pq.empty()) {
        Vertex<T>* current = pq.top();
        pq.pop();

        if (current->isVisited()) continue;
        current->setVisited(true);

        // std::cout << "Processing node: " << current->getInfo() << " (dist=" << current->getDist() << ")\n";

        if (current == destVert) {
            // std::cout << "Reached destination!\n";
            break;
        }

        for (Edge<T>* edge : current->getAdj()) {
            Vertex<T>* neighbor = edge->getDest();
            if (edge->getDriveTime() == INF || !edge->isAvailable() || !neighbor->isAvailable()) continue; // ignore
            double new_dist = current->getDist() + edge->getDriveTime();
            if (new_dist < neighbor->getDist()) {
                neighbor->setDist(new_dist);
                neighbor->setPath(edge);
                pq.push(neighbor);
                // std::cout << "  Updating neighbor " << neighbor->getInfo()
                //          << " with new dist=" << new_dist << "\n";
            }
        }
    }

    // need to reconstruct path
    std::vector<Edge<T>*> path;
    if (destVert->getPath() == nullptr) {
        // std::cout << "No path found from " << origin << " to " << destination << "!\n";
        return {};
    }


    for (Edge<T>* e = destVert->getPath(); e != nullptr; e = e->getOrigin()->getPath()) {
        path.push_back(e);
    }

    std::reverse(path.begin(), path.end());

    return path;
}


/**
 * @brief Retrieves all vertices that have parking available.
 * 
 * @return std::vector<Vertex<T>*> A vector containing pointers to all vertices with parking.
 * 
 * @details
 * - Iterates through the `idToVertexMap`, checking if each vertex has parking.
 * - If a vertex has parking (`hasParking() == true`), it is added to the result vector.
 * - Returns a list of all parking vertices.
 * 
 * @complexity
 * - **Iteration over all vertices:** O(V)
 * - **Total Complexity:** O(V)
 */
template <class T>
std::vector<Vertex<T>*> Graph<T>::getAllParkingVertices() const {
    std::vector<Vertex<T>*> result;
    for (const auto& pair : idToVertexMap) {
        if (pair.second->hasParking()) result.push_back(pair.second);
    }
    return result;
}


/**
 * @brief Computes the shortest walking path between two vertices using Dijkstra's algorithm.
 * 
 * @param origin The starting vertex.
 * @param destination The target vertex.
 * @return std::vector<Edge<T>*> A vector representing the shortest path edges in order.
 * 
 * @details
 * - Uses a **priority queue (min-heap)** to efficiently process vertices in increasing distance order.
 * - Initializes all vertex distances to **infinity (INF)** and resets their paths.
 * - Extracts the vertex with the smallest distance, updating its neighbors accordingly.
 * - Stops early if the destination is reached.
 * - Returns a vector of edges forming the shortest walking path.
 * 
 * @complexity
 * - **Priority queue operations (O(log V))** per vertex.
 * - **Each edge is processed once (O(E log V))**.
 * - **Overall Complexity:** **O((V + E) log V)**.
 */
template <class T>
std::vector<Edge<T>*> Graph<T>::dijkstraWalking(const T& origin, const T& destination) {
    std::priority_queue<Vertex<T>*, std::vector<Vertex<T>*>, vertexComp<T>> pq;

    for (auto& it : this->idToVertexMap) {
        it.second->setDist(INF);
        it.second->setPath(nullptr);
        it.second->setVisited(false);
    }

    auto it = idToVertexMap.find(origin);
    if (it == idToVertexMap.end()) return {};

    auto it2 = idToVertexMap.find(destination);
    if (it2 == idToVertexMap.end()) return {};

    Vertex<T>* originVert = it->second;
    Vertex<T>* destVert = it2->second;

    originVert->setDist(0);
    pq.push(originVert);

    while (!pq.empty()) {
        Vertex<T>* current = pq.top();
        pq.pop();

        if (current->isVisited()) continue;
        current->setVisited(true);

        if (current == destVert) break;

        for (Edge<T>* edge : current->getAdj()) {
            Vertex<T>* neighbor = edge->getDest();
            if (edge->getWalkTime() == INF) continue;
            double new_dist = current->getDist() + edge->getWalkTime();
            if (new_dist < neighbor->getDist()) {
                neighbor->setDist(new_dist);
                neighbor->setPath(edge);
                pq.push(neighbor);
            }
        }
    }

    std::vector<Edge<T>*> path;
    if (destVert->getPath() == nullptr) return {};

    for (Edge<T>* e = destVert->getPath(); e != nullptr; e = e->getOrigin()->getPath()) {
        path.push_back(e);
    }

    std::reverse(path.begin(), path.end());
    return path;
}


/**
 * @brief Finds the fastest restricted driving path from origin to destination, optionally passing through a stop point.
 * 
 * This function computes the fastest restricted driving path from an origin to a destination vertex. It considers
 * avoiding specific nodes and segments, which are excluded from the path. The function can also handle an optional
 * stop node that the path should pass through. The function uses Dijkstra's algorithm for finding the shortest
 * paths between the origin, stop, and destination, and excludes certain edges during the process.
 * 
 * @param origin The origin vertex.
 * @param destination The destination vertex.
 * @param avoidNodes A list of vertices to be excluded from the path.
 * @param avoidSegments A list of edges (represented as pairs of vertices) to be excluded from the path.
 * @param stop An optional stop vertex that the path must pass through. If no stop is provided, the path is calculated
 *             directly from origin to destination.
 * 
 * @return void This function doesn't return any value, but it outputs the result (path) to the standard output and 
 *         a file.
 * 
 * @note The function modifies the availability status of vertices and edges based on the provided `avoidNodes` and 
 *       `avoidSegments`. It restores the availability status of the affected vertices and edges after computation.
 * 
 * @complexity
 * - Excluding nodes and segments: O(N * E), where N is the number of nodes (vertices) and E is the number of edges.
 * - Dijkstra’s algorithm: O((V + E) log V) for each call, where V is the number of vertices and E is the number of edges.
 * - The total complexity is therefore approximately O(N * E + 3 * (V + E) log V), where:
 *    - N is the number of vertices in the avoidNodes list.
 *    - E is the number of edges in the avoidSegments list.
 *    - V and E represent the total number of vertices and edges in the graph.
 */
template <class T>
void Graph<T>::fastestRestrictedDrivingPath(const T& origin, const T& destination, std::vector<T> avoidNodes, std::vector<std::pair<T,T>> avoidSegments, std::optional<T> stop) {
    std::vector<Edge<T>*> path;
    std::ofstream file("../output.txt");

    // exclude requested nodes
    for (auto node : avoidNodes) {
        Vertex<T>* vert = findVertex(node);
        if (vert == nullptr) {
            continue; // assume typo if not found
        }
        vert->setAvailable(false);
    }

    std::vector<Edge<T>*> switchedEdges; // we store edges here to switch them back to available without long lookups
    // exclude requested segments
    for (const auto& pair : avoidSegments) {
        Vertex<T>* orig = findVertex(pair.first);
        if (orig == nullptr || findVertex(pair.second) == nullptr) {
            continue;
        }

        for (auto edge : orig->getAdj()) { // this has time complexity O(e) and could be made faster if we used an unordered_map for adj,
            if (edge->getDest()->getInfo() == pair.second) { // but since the graph is not too big the performance gain would be negligible
                edge->setAvailable(false);
                switchedEdges.push_back(edge);
            }
        }
    }
    
    if (!stop.has_value()) {
        path = dijkstraDriving(origin, destination);
        std::cout << "Source:" << origin << "\n";
        std::cout << "Destination:" << destination << "\n";
        std::cout << "RestrictedDrivingRoute:";

        file << "Source:" << origin << "\n";
        file << "Destination:" << destination << "\n";
        file << "RestrictedDrivingRoute:";
        for (auto edge : path) {std::cout << edge->getOrigin()->getInfo() << ","; file << edge->getOrigin()->getInfo() << ",";}
        std::cout << destination << "(" << path.back()->getDest()->getDist() << ")\n";
        file << destination << "(" << path.back()->getDest()->getDist() << ")\n";

        for (auto node : avoidNodes) {
            Vertex<T>* vert = findVertex(node);
            if (vert == nullptr) {
                continue; // assume typo if not found
            }
            vert->setAvailable(true);
        }

        for (auto edge : switchedEdges) edge->setAvailable(true);
        file.close();
        return;
    }

    /* if (avoidNodes.count(stop.value())) {
        throw std::runtime_error("Error: Mandatory stop with cannot be in excluded nodes set!");
    } */

    // Step 1: Find shortest path from origin → stop
    std::vector<Edge<T>*> firstHalf = dijkstraDriving(origin, stop.value());
    if (firstHalf.empty()) {
        std::cout << "Source:" << origin << "\n";
        std::cout << "Destination:" << destination << "\n";
        std::cout << "RestrictedDrivingRoute:none\n";

        file << "Source:" << origin << "\n";
        file << "Destination:" << destination << "\n";
        file << "RestrictedDrivingRoute:none\n";

        for (auto node : avoidNodes) {
            Vertex<T>* vert = findVertex(node);
            if (vert == nullptr) {
                continue; // assume typo if not found
            }
            vert->setAvailable(true);
        }

        for (auto edge : switchedEdges) edge->setAvailable(true);
        file.close();
        return;
    }
    double halfwayDist = firstHalf.back()->getDest()->getDist(); // we store the distance now, before it gets reset

    // Step 2: Find shortest path from stop -> destination
    std::vector<Edge<T>*> secondHalf = dijkstraDriving(stop.value(), destination);
    if (secondHalf.empty()) {
        std::cout << "Source:" << origin << "\n";
        std::cout << "Destination:" << destination << "\n";
        std::cout << "RestrictedDrivingRoute:none\n";
        file << "Source:" << origin << "\n";
        file << "Destination:" << destination << "\n";
        file << "RestrictedDrivingRoute:none\n";

        for (auto node : avoidNodes) {
            Vertex<T>* vert = findVertex(node);
            if (vert == nullptr) {
                continue; // assume typo if not found
            }
            vert->setAvailable(true);
        }

        for (auto edge : switchedEdges) edge->setAvailable(true);
        file.close();
        return;
    }

    double finalDist = secondHalf.back()->getDest()->getDist();

    firstHalf.insert(firstHalf.end(), secondHalf.begin(), secondHalf.end());

    std::cout << "Source:" << origin << "\n";
    std::cout << "Destination:" << destination << "\n";
    std::cout << "RestrictedDrivingRoute:";
    file << "Source:" << origin << "\n";
    file << "Destination:" << destination << "\n";
    file << "RestrictedDrivingRoute:";

    for (auto edge : firstHalf) {std::cout << edge->getOrigin()->getInfo() << ","; file << edge->getOrigin()->getInfo() << ",";}
    std::cout << destination << "(" << halfwayDist + finalDist << ")\n";
    file << destination << "(" << halfwayDist + finalDist << ")\n";
    
    for (auto node : avoidNodes) {
        Vertex<T>* vert = findVertex(node);
        if (vert == nullptr) {
            continue; // assume typo if not found
        }
        vert->setAvailable(true);
    }

    for (auto edge : switchedEdges) edge->setAvailable(true);

    return;
}

#endif
