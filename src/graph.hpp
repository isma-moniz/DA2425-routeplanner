#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include <queue>
#include <limits>
#include <algorithm>
#include <unordered_map>

template <class T>
class Edge;

#define INF std::numeric_limits<double>::max()

/******************** Vertex ********************/

template <class T>
class Vertex {
public:
    Vertex(T in, bool parking);
    bool operator<(Vertex<T>& vertex) const; // comparison operator overload
    
    T getInfo() const;
    std::vector<Edge<T> *> getAdj() const;
    bool isVisited() const;
    bool isProcessing() const;
    bool hasParking() const;
    unsigned int getIndegree() const;
    double getDist() const;
    Edge<T>* getPath() const;
    std::vector<Edge<T>*> getIncoming() const;

    void setInfo(T info);
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
    bool removeEdge(T in);
    void removeOutgoingEdges();

    // friend class MutablePriorityQueue<Vertex>; -- probably not necessary yet
protected:
    T info; // we will store the id
    std::vector<Edge<T> *> adj;

    bool visited = false; // used by DFS, BFS, Primm
    bool processing = false; // used to detect cycles
    bool parkingSpace = false;
    
    int low = -1, num = -1; // used by SCC Tarjan
    unsigned int indegree; // used by topsort
    double dist = 0;
    Edge<T>* path = nullptr;

    std::vector<Edge<T>*> incoming;
    int queueIndex = 0; // for MutablePriorityQueue and UFDS
    
    void deleteEdge(Edge<T>* edge);
};

/******************** Edge ********************/

template <class T>
class Edge {
public:
    Edge(Vertex<T>* orig, Vertex<T> *dest, double w, double d);

    Vertex<T>* getDest() const;
    double getDriveTime() const;
    double getWalkTime() const;
    bool isSelected() const;
    Vertex<T>* getOrigin() const;
    Edge<T>* getReverse() const;
    double getFlow() const;

    void setSelected(bool selected);
    void setReverse(Edge<T>* reverse);
    void setFlow(double flow);
protected:
    Vertex<T>* dest;
    double walkTime;
    double driveTime;

    bool selected = false;

    Vertex<T>* origin;
    Edge<T>* reverse = nullptr;

    double flow; // for flow related problems
};

/******************** Graph ********************/

template <class T>
class Graph {
public:
    ~Graph();
    // Aux function to find a vertex with a given content.
    Vertex<T>* findVertex(const T& in) const;
    // Add vertex with a given info to graph.
    bool addVertex(const T& in);
    bool removeVertex(const T& in);
    // add edge to the graph given contents of source and dest as well as weights
    bool addEdge(const T& source, const T& dest, double w, double d);
    bool removeEdge(const T& source, const T& dest);
    bool addBidirectionalEdge(const T &source, const T& dest, double w, double d);

    int getNumVertex() const;
    std::vector<Vertex<T>*> getVertexSet() const;

protected:
    std::vector<Vertex<T>*> vertexSet; 
    std::unordered_map<std::string, Vertex<T>*> codeToVertexMap; // provides constant time lookup by vertex code
    
    // matrixes for Floyd-Warshall
    double** distMatrix = nullptr;
    int** pathMatrix = nullptr;

    int findVertexIdx(const T& in) const;
};

void deleteMatrix(int** m, int n);
void deleteMatrix(double** m, int n);

#endif
