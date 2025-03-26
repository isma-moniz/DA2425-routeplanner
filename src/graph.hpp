#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>

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
    string code;
    vector<Edge<T>*> adj;
    vector<Edge<T>*> incoming;
    bool visited = false;
    bool processing = false;
    bool parkingSpace = false;
    unsigned int indegree = 0;
    double dist = 0;
    Edge<T>* path = nullptr;
    int low = 0;
    int num = 0;
public:
    Vertex(T in, const string& code, bool parking) : info(in), code(code), parkingSpace(parking) {}
    T getInfo() const { return info; }
    const string& getCode() const { return code; }
    void setCode(const string& c) { code = c; }
    vector<Edge<T>*> getAdj() const { return adj; }
    bool isVisited() const { return visited; }
    bool isProcessing() const { return processing; }
    bool hasParking() const { return parkingSpace; }
    unsigned int getIndegree() const { return indegree; }
    double getDist() const { return dist; }
    Edge<T>* getPath() const { return path; }
    vector<Edge<T>*> getIncoming() const { return incoming; }
    int getLow() const { return low; }
    int getNum() const { return num; }
    void setLow(int value) { low = value; }
    void setNum(int value) { num = value; }
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
    bool removeEdge(T in) {
        bool removedEdge = false;
        auto it = adj.begin();
        while (it != adj.end()) {
            Edge<T> *edge = *it;
            Vertex<T> *dest = edge->getDest();
            if (dest->getInfo() == in) {
                it = adj.erase(it);
                deleteEdge(edge);
                removedEdge = true;
            } else {
                it++;
            }
        }
        return removedEdge;
    }
    void removeOutgoingEdges() {
        auto it = adj.begin();
        while (it != adj.end()) {
            Edge<T> *edge = *it;
            it = adj.erase(it);
            deleteEdge(edge);
        }
    }
    void deleteEdge(Edge<T> *edge) {
        Vertex<T> *dest = edge->getDest();
        auto it = dest->incoming.begin();
        while (it != dest->incoming.end()) {
            if ((*it)->getOrigin()->getInfo() == info) {
                it = dest->incoming.erase(it);
            } else {
                it++;
            }
        }
        delete edge;
    }
    bool operator<(Vertex<T>& vertex) const { return dist < vertex.dist; }
};

template <class T>
class Edge {
    Vertex<T> *origin;
    Vertex<T> *dest;
    double walkTime;
    double driveTime;
    Edge<T> *reverse = nullptr;
    bool selected = false;
    double flow = 0;
public:
    Edge(Vertex<T> *orig, Vertex<T> *dest, double w, double d) : origin(orig), dest(dest), walkTime(w), driveTime(d) {}
    Vertex<T>* getDest() const { return dest; }
    double getDriveTime() const { return driveTime; }
    double getWalkTime() const { return walkTime; }
    Vertex<T>* getOrigin() const { return origin; }
    Edge<T>* getReverse() const { return reverse; }
    bool isSelected() const { return selected; }
    double getFlow() const { return flow; }
    void setSelected(bool s) { selected = s; }
    void setReverse(Edge<T> *r) { reverse = r; }
    void setFlow(double f) { flow = f; }
};

template <class T>
class Graph {
    unordered_map<T, Vertex<T>*> idToVertexMap;
    unordered_map<string, Vertex<T>*> codeToVertexMap;
    double **distMatrix = nullptr;
    int **pathMatrix = nullptr;
public:
    ~Graph() {
        deleteMatrix(distMatrix, idToVertexMap.size());
        deleteMatrix(pathMatrix, idToVertexMap.size());
    }
    int getNumVertex() const { return idToVertexMap.size(); }
    vector<Vertex<T>*> getVertexSet() const {
        vector<Vertex<T>*> vertices;
        for (const auto& pair : idToVertexMap) {
            vertices.push_back(pair.second);
        }
        return vertices;
    }
    Vertex<T>* findVertex(const T &in) const {
        auto it = idToVertexMap.find(in);
        return (it != idToVertexMap.end()) ? it->second : nullptr;
    }
    Vertex<T>* findVertex(const string& code) const {
        auto it = codeToVertexMap.find(code);
        return (it != codeToVertexMap.end()) ? it->second : nullptr;
    }
    bool addVertex(const T &in, const string& code, bool parking) {
        if (idToVertexMap.find(in) != idToVertexMap.end()) return false;
        Vertex<T>* v = new Vertex<T>(in, code, parking);
        idToVertexMap[in] = v;
        codeToVertexMap[code] = v;
        return true;
    }
    bool removeVertex(const T &in) {
        auto it = idToVertexMap.find(in);
        if (it == idToVertexMap.end()) return false;
        Vertex<T>* v = it->second;
        v->removeOutgoingEdges();
        for (Edge<T>* e : v->getIncoming()) {
            e->getOrigin()->removeEdge(in);
        }
        idToVertexMap.erase(it);
        codeToVertexMap.erase(v->getCode());
        delete v;
        return true;
    }
    bool addEdge(const T &sourc, const T &dest, double w, double d) {
        auto v1 = findVertex(sourc);
        auto v2 = findVertex(dest);
        if (v1 == nullptr || v2 == nullptr) return false;
        v1->addEdge(v2, w, d);
        return true;
    }
    bool removeEdge(const T &sourc, const T &dest) {
        Vertex<T> * srcVertex = findVertex(sourc);
        if (srcVertex == nullptr) return false;
        return srcVertex->removeEdge(dest);
    }
    bool addBidirectionalEdge(const T &sourc, const T &dest, double w, double d) {
        auto v1 = findVertex(sourc);
        auto v2 = findVertex(dest);
        if (v1 == nullptr || v2 == nullptr) return false;
        auto e1 = v1->addEdge(v2, w, d);
        auto e2 = v2->addEdge(v1, w, d);
        e1->setReverse(e2);
        e2->setReverse(e1);
        return true;
    }
};

#endif