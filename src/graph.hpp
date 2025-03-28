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

inline void deleteMatrix(int **m, int n) {
    if (m != nullptr) {
        for (int i = 0; i < n; i++)
            if (m[i] != nullptr)
                delete [] m[i];
        delete [] m;
    }
}

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

template <class T>
class Vertex {
    T info;
    std::vector<Edge<T>*> adj;
    std::vector<Edge<T>*> incoming;
    bool visited = false;
    bool processing = false;
    bool parkingSpace = false;
    unsigned int indegree = 0;
    double dist = 0;
    Edge<T>* path = nullptr;
    int low = 0;
    int num = 0;
public:
    Vertex(T in, bool parking) : info(in), parkingSpace(parking) {}
    T getInfo() const { return info; }
    std::vector<Edge<T>*> getAdj() const { return adj; }
    bool isVisited() const { return visited; }
    bool isProcessing() const { return processing; }
    bool hasParking() const { return parkingSpace; }
    unsigned int getIndegree() const { return indegree; }
    double getDist() const { return dist; }
    Edge<T>* getPath() const { return path; }
    std::vector<Edge<T>*> getIncoming() const { return incoming; }
    void setInfo(T in) { info = in; }
    void setVisited(bool v) { visited = v; }
    void setProcessing(bool p) { processing = p; }
    void setParking(bool p) { parkingSpace = p; }
    void setIndegree(unsigned int i) { indegree = i; }
    void setDist(double d) { dist = d; }
    void setPath(Edge<T> *p) { path = p; }
    Edge<T>* addEdge(Vertex<T> *dest, double w, double d) {
        auto newEdge = new Edge<T>(this, dest, w, d);
        adj.push_back(newEdge);
        dest->incoming.push_back(newEdge);
        return newEdge;
    }
    void removeOutgoingEdges() {
        auto it = adj.begin();
        while (it != adj.end()) {
            Edge<T> *edge = *it;
            it = adj.erase(it);
            delete edge;
        }
    }
};

template <class T>
class Edge {
    Vertex<T> *origin;
    Vertex<T> *dest;
    double walkTime;
    double driveTime;
public:
    Edge(Vertex<T> *orig, Vertex<T> *dest, double w, double d) : origin(orig), dest(dest), walkTime(w), driveTime(d) {}
    Vertex<T>* getDest() const { return dest; }
    double getDriveTime() const { return driveTime; }
    double getWalkTime() const { return walkTime; }
    Vertex<T>* getOrigin() const { return origin; }
};

template <class T>
class Graph {
    std::unordered_map<T, Vertex<T>*> vertexMap;
public:

    int getNumVertex() const { return vertexMap.size(); }
    std::vector<Vertex<T>*> getVertexSet() const {
        std::vector<Vertex<T>*> vertices;
        for (const auto& pair : vertexMap) {
            vertices.push_back(pair.second);
        }
        return vertices;
    }
    Vertex<T>* findVertex(const T &in) const {
        auto it = vertexMap.find(in);
        return (it != vertexMap.end()) ? it->second : nullptr;
    }
    bool addVertex(const T &in, bool parking) {
        if (vertexMap.find(in) != vertexMap.end()) return false;
        Vertex<T>* v = new Vertex<T>(in, parking);
        vertexMap[in] = v;
        return true;
    }
    bool addBidirectionalEdge(const T &sourc, const T &dest, double w, double d) {
        auto v1 = findVertex(sourc);
        auto v2 = findVertex(dest);
        if (v1 == nullptr || v2 == nullptr) return false;
        v1->addEdge(v2, w, d);
        v2->addEdge(v1, w, d);
        return true;
    }
    void dijkstraDriving(const T &origin) {
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
