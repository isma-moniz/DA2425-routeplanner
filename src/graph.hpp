#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <queue>
#include <limits>
#include <functional>
#include <utility>

using namespace std;

/**
 * @brief Deletes a dynamically allocated matrix of integers.
 * @param m Pointer to the matrix.
 * @param n Number of rows in the matrix.
 * @details This function will delete each row of the matrix and then delete the matrix itself.
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
 * @brief Deletes a dynamically allocated matrix of doubles.
 * @param m Pointer to the matrix.
 * @param n Number of rows in the matrix.
 * @details This function will delete each row of the matrix and then delete the matrix itself.
 */
inline void deleteMatrix(double **m, int n) {
    if (m != nullptr) {
        for (int i = 0; i < n; i++)
            if (m[i] != nullptr)
                delete [] m[i];
        delete [] m;
    }
}

template <class T> class Edge;
template <class T> class Graph;

/**
 * @class Vertex
 * @brief Represents a vertex in a graph.
 * @tparam T The type of data stored in the vertex.
 */
template <class T>
class Vertex {
    T info; /**< Information stored in the vertex */
    std::vector<Edge<T>*> adj; /**< Adjacency list of outgoing edges */
    std::vector<Edge<T>*> incoming; /**< Adjacency list of incoming edges */
    bool visited = false; /**< Flag indicating if the vertex has been visited */
    bool processing = false; /**< Flag indicating if the vertex is being processed */
    bool parkingSpace = false; /**< Flag indicating if the vertex has a parking space */
    unsigned int indegree = 0; /**< The number of incoming edges */
    double dist = 0; /**< The distance value used for algorithms like Dijkstra */
    Edge<T>* path = nullptr; /**< The path edge leading to this vertex */
    int low = 0; /**< A value used in DFS-based algorithms */
    int num = 0; /**< A number used for DFS ordering */
    
public:
    /**
     * @brief Constructor to create a Vertex.
     * @param in The information of the vertex.
     * @param parking Boolean indicating if the vertex has a parking space.
     */
    Vertex(T in, bool parking) : info(in), parkingSpace(parking) {}

    /**
     * @brief Retrieves the information stored in the vertex.
     * @return The information of the vertex.
     */
    T getInfo() const { return info; }

    /**
     * @brief Retrieves the adjacency list of outgoing edges.
     * @return A vector of edges representing outgoing connections.
     */
    std::vector<Edge<T>*> getAdj() const { return adj; }

    /**
     * @brief Checks if the vertex has been visited.
     * @return True if visited, false otherwise.
     */
    bool isVisited() const { return visited; }

    /**
     * @brief Checks if the vertex is being processed.
     * @return True if being processed, false otherwise.
     */
    bool isProcessing() const { return processing; }

    /**
     * @brief Checks if the vertex has a parking space.
     * @return True if it has a parking space, false otherwise.
     */
    bool hasParking() const { return parkingSpace; }

    /**
     * @brief Retrieves the indegree of the vertex.
     * @return The number of incoming edges.
     */
    unsigned int getIndegree() const { return indegree; }

    /**
     * @brief Retrieves the distance value of the vertex.
     * @return The distance of the vertex.
     */
    double getDist() const { return dist; }

    /**
     * @brief Retrieves the edge representing the path to the vertex.
     * @return The edge that represents the path.
     */
    Edge<T>* getPath() const { return path; }

    /**
     * @brief Retrieves the list of incoming edges.
     * @return A vector of incoming edges.
     */
    std::vector<Edge<T>*> getIncoming() const { return incoming; }

    /**
     * @brief Sets the information stored in the vertex.
     * @param in The new information to store in the vertex.
     */
    void setInfo(T in) { info = in; }

    /**
     * @brief Sets the visited flag of the vertex.
     * @param v The visited state to set.
     */
    void setVisited(bool v) { visited = v; }

    /**
     * @brief Sets the processing flag of the vertex.
     * @param p The processing state to set.
     */
    void setProcessing(bool p) { processing = p; }

    /**
     * @brief Sets whether the vertex has a parking space.
     * @param p The parking state to set.
     */
    void setParking(bool p) { parkingSpace = p; }

    /**
     * @brief Sets the indegree of the vertex.
     * @param i The new indegree value.
     */
    void setIndegree(unsigned int i) { indegree = i; }

    /**
     * @brief Sets the distance value of the vertex.
     * @param d The new distance value.
     */
    void setDist(double d) { dist = d; }

    /**
     * @brief Sets the path edge leading to the vertex.
     * @param p The new edge representing the path to the vertex.
     */
    void setPath(Edge<T> *p) { path = p; }

    /**
     * @brief Adds an edge to the vertex, connecting it to a destination vertex.
     * @param dest The destination vertex.
     * @param w The walking time of the edge.
     * @param d The driving time of the edge.
     * @return A pointer to the created edge.
     */
    Edge<T>* addEdge(Vertex<T> *dest, double w, double d) {
        auto newEdge = new Edge<T>(this, dest, w, d);
        adj.push_back(newEdge);
        dest->incoming.push_back(newEdge);
        return newEdge;
    }

    /**
     * @brief Removes all outgoing edges from the vertex.
     * @details This function deletes the outgoing edges and clears the adjacency list.
     */
    void removeOutgoingEdges() {
        auto it = adj.begin();
        while (it != adj.end()) {
            Edge<T> *edge = *it;
            it = adj.erase(it);
            delete edge;
        }
    }
};

/**
 * @class Edge
 * @brief Represents an edge between two vertices in a graph.
 * @tparam T The type of data stored in the vertex.
 */
template <class T>
class Edge {
    Vertex<T> *origin; /**< The origin vertex of the edge */
    Vertex<T> *dest; /**< The destination vertex of the edge */
    double walkTime; /**< The walking time associated with the edge */
    double driveTime; /**< The driving time associated with the edge */

public:
    /**
     * @brief Constructor to create an edge between two vertices.
     * @param orig The origin vertex.
     * @param dest The destination vertex.
     * @param w The walking time.
     * @param d The driving time.
     */
    Edge(Vertex<T> *orig, Vertex<T> *dest, double w, double d) : origin(orig), dest(dest), walkTime(w), driveTime(d) {}

    /**
     * @brief Retrieves the destination vertex of the edge.
     * @return The destination vertex.
     */
    Vertex<T>* getDest() const { return dest; }

    /**
     * @brief Retrieves the driving time associated with the edge.
     * @return The driving time.
     */
    double getDriveTime() const { return driveTime; }

    /**
     * @brief Retrieves the walking time associated with the edge.
     * @return The walking time.
     */
    double getWalkTime() const { return walkTime; }

    /**
     * @brief Retrieves the origin vertex of the edge.
     * @return The origin vertex.
     */
    Vertex<T>* getOrigin() const { return origin; }
};

/**
 * @class Graph
 * @brief Represents a graph with vertices and edges.
 * @tparam T The type of data stored in the vertices.
 */
template <class T>
class Graph {
    std::unordered_map<T, Vertex<T>*> vertexMap; /**< Map of vertices in the graph */

public:
    /**
     * @brief Retrieves the number of vertices in the graph.
     * @return The number of vertices.
     */
    int getNumVertex() const { return vertexMap.size(); }

    /**
     * @brief Retrieves all the vertices in the graph.
     * @return A vector containing all the vertices.
     */
    std::vector<Vertex<T>*> getVertexSet() const {
        std::vector<Vertex<T>*> vertices;
        for (const auto& pair : vertexMap) {
            vertices.push_back(pair.second);
        }
        return vertices;
    }

    /**
     * @brief Finds a vertex by its information.
     * @param in The information of the vertex.
     * @return A pointer to the vertex if found, nullptr otherwise.
     */
    Vertex<T>* findVertex(const T &in) const {
        auto it = vertexMap.find(in);
        return (it != vertexMap.end()) ? it->second : nullptr;
    }

    /**
     * @brief Adds a vertex to the graph.
     * @param in The information to store in the vertex.
     * @param parking Boolean indicating if the vertex has a parking space.
     * @return True if the vertex was added successfully, false if it already exists.
     */
    bool addVertex(const T &in, bool parking) {
        if (vertexMap.find(in) != vertexMap.end()) return false;
        Vertex<T>* v = new Vertex<T>(in, parking);
        vertexMap[in] = v;
        return true;
    }

    /**
     * @brief Adds a bidirectional edge between two vertices.
     * @param sourc The source vertex information.
     * @param dest The destination vertex information.
     * @param w The walking time of the edge.
     * @param d The driving time of the edge.
     * @return True if the edge was added successfully, false if either vertex is not found.
     */
    bool addBidirectionalEdge(const T &sourc, const T &dest, double w, double d) {
        auto v1 = findVertex(sourc);
        auto v2 = findVertex(dest);
        if (v1 == nullptr || v2 == nullptr) return false;
        v1->addEdge(v2, w, d);
        v2->addEdge(v1, w, d);
        return true;
    }

    /**
     * @brief Performs Dijkstra's algorithm to find the shortest driving times from the origin vertex.
     * @param origin The information of the origin vertex.
     */
    void dijkstraDriving(const T &origin) {
        // Reset all vertices
        for (auto &pair : vertexMap) {
            pair.second->setDist(std::numeric_limits<double>::max());
            pair.second->setPath(nullptr);
            pair.second->setVisited(false);
        }

        auto start = findVertex(origin);
        if (!start) return;

        start->setDist(0);
        std::priority_queue<std::pair<double, Vertex<T>*>, std::vector<std::pair<double, Vertex<T>*>>, std::greater<std::pair<double, Vertex<T>*>>> pq;
        pq.push(std::make_pair(0, start));

        // Dijkstra's algorithm loop
        while (!pq.empty()) {
            auto topElement = pq.top();
            pq.pop();

            double currDist = topElement.first;
            Vertex<T>* currVertex = topElement.second;

            if (currVertex->isVisited()) continue;
            currVertex->setVisited(true);

            for (auto edge : currVertex->getAdj()) {
                if (edge->getDriveTime() < 0) continue;

                auto dest = edge->getDest();
                double newDist = currVertex->getDist() + edge->getDriveTime();

                if (newDist < dest->getDist()) {
                    dest->setDist(newDist);
                    dest->setPath(edge);
                    pq.push(std::make_pair(newDist, dest));
                }
            }
        }
    }
};

#endif
