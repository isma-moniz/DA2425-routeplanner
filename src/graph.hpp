#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include <unordered_map>

template <class T>
class Edge;

#define INF std::numeric_limits<double>::max()

/******************** Vertex ********************/

template <class T>
class Vertex {
public:
    Vertex(T in, const std::string& code, bool parking);
    bool operator<(Vertex<T>& vertex) const; // comparison operator overload
    
    T getInfo() const;
    const std::string& getCode() const;
    std::vector<Edge<T> *> getAdj() const;
    bool isVisited() const;
    bool isProcessing() const;
    bool hasParking() const;
    unsigned int getIndegree() const;
    double getDist() const;
    Edge<T>* getPath() const;
    std::vector<Edge<T>*> getIncoming() const;

    void setInfo(T info);
    void setCode(const std::string& code);
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
    std::string code;
    std::vector<Edge<T> *> adj; // if we don't ever need to have the edges ordered, we will change this to unordered_set for constant time lookup, add and remove

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
    // Aux function to find a vertex with a given code.
    Vertex<T>* findVertex(const std::string& code) const;
    // Add vertex with a given info to graph.
    bool addVertex(const T& in, const std::string& code, bool parking);
    bool removeVertex(const T& in);
    // add edge to the graph given contents of source and dest as well as weights
    bool addEdge(const T& source, const T& dest, double w, double d);
    bool removeEdge(const T& source, const T& dest);
    bool addBidirectionalEdge(const T &source, const T& dest, double w, double d);

    int getNumVertex() const;
    std::vector<Vertex<T>*> getVertexSet() const;

    void dijkstraDriving(const T& origin);

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

/*
 * Basic constructor
 */
template <class T>
Vertex<T>::Vertex(T in, const std::string& code, bool parking): info(in), code(code), parkingSpace(parking) {}

/*
 * Auxiliary function to add an outgoing edge to a vertex (this),
 * with a given destination vertex (d), edge walking (w) and driving (d) times.
 */
template <class T>
Edge<T> * Vertex<T>::addEdge(Vertex<T> *dest, double w, double d) {
    auto newEdge = new Edge<T>(this, dest, w, d);
    adj.push_back(newEdge);
    dest->incoming.push_back(newEdge);
    return newEdge;
}

/*
 * Auxiliary function to remove an outgoing edge (with a given destination (d))
 * from a vertex (this).
 * Returns true if successful, and false if such edge does not exist.
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

/*
 * Auxiliary function to remove an outgoing edge of a vertex.
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

template <class T>
bool Vertex<T>::operator<(Vertex<T>& vertex) const {
    return this->dist < vertex.dist;
}

template <class T>
T Vertex<T>::getInfo() const {
    return this->info;
}

template <class T>
const std::string& Vertex<T>::getCode() const {
    return this->code;
}

template <class T>
void Vertex<T>::setCode(const std::string& code) {
    this->code = code;
}

template <class T>
int Vertex<T>::getLow() const {
    return this->low;
}

template <class T>
void Vertex<T>::setLow(int value) {
    this->low = value;
}

template <class T>
int Vertex<T>::getNum() const {
    return this->num;
}

template <class T>
void Vertex<T>::setNum(int value) {
    this->num = value;
}

template <class T>
std::vector<Edge<T>*> Vertex<T>::getAdj() const {
    return this->adj;
}

template <class T>
bool Vertex<T>::isVisited() const {
    return this->visited;
}

template <class T>
bool Vertex<T>::isProcessing() const {
    return this->processing;
}

template <class T>
bool Vertex<T>::hasParking() const {
    return this->parkingSpace;
}

template <class T>
unsigned int Vertex<T>::getIndegree() const {
    return this->indegree;
}

template <class T>
double Vertex<T>::getDist() const {
    return this->dist;
}

template <class T>
Edge<T> *Vertex<T>::getPath() const {
    return this->path;
}

template <class T>
std::vector<Edge<T> *> Vertex<T>::getIncoming() const {
    return this->incoming;
}

template <class T>
void Vertex<T>::setInfo(T in) {
    this->info = in;
}

template <class T>
void Vertex<T>::setVisited(bool visited) {
    this->visited = visited;
}

template <class T>
void Vertex<T>::setProcessing(bool processing) {
    this->processing = processing;
}

template <class T>
void Vertex<T>::setParking(bool parking) {
    this->parkingSpace = parking;
}

template <class T>
void Vertex<T>::setIndegree(unsigned int indegree) {
    this->indegree = indegree;
}

template <class T>
void Vertex<T>::setDist(double dist) {
    this->dist = dist;
}

template <class T>
void Vertex<T>::setPath(Edge<T> *path) {
    this->path = path;
}

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

template <class T>
Edge<T>::Edge(Vertex<T> *orig, Vertex<T> *dest, double w, double d): origin(orig), dest(dest), walkTime(w), driveTime(d) {}

template <class T>
Vertex<T> * Edge<T>::getDest() const {
    return this->dest;
}

template <class T>
double Edge<T>::getDriveTime() const {
    return this->driveTime;
}

template <class T>
double Edge<T>::getWalkTime() const {
    return this->walkTime;
}

template <class T>
Vertex<T> * Edge<T>::getOrigin() const {
    return this->origin;
}

template <class T>
Edge<T> *Edge<T>::getReverse() const {
    return this->reverse;
}

template <class T>
bool Edge<T>::isSelected() const {
    return this->selected;
}

template <class T>
double Edge<T>::getFlow() const {
    return flow;
}

template <class T>
void Edge<T>::setSelected(bool selected) {
    this->selected = selected;
}

template <class T>
void Edge<T>::setReverse(Edge<T> *reverse) {
    this->reverse = reverse;
}

template <class T>
void Edge<T>::setFlow(double flow) {
    this->flow = flow;
}

/********************** Graph  ****************************/

template <class T>
int Graph<T>::getNumVertex() const {
    return idToVertexMap.size(); 
}

template <class T>
std::vector<Vertex<T> *> Graph<T>::getVertexSet() const {
    std::vector<Vertex<T>*> vertices;
    for (const auto& pair : idToVertexMap) {
        vertices.push_back(pair.second);
    }
    return vertices;
}

template <class T>
Vertex<T>* Graph<T>::findVertex(const T &in) const {
    auto it = idToVertexMap.find(in);
    return (it != idToVertexMap.end()) ? it->second : nullptr;
}

template <class T>
Vertex<T>* Graph<T>::findVertex(const std::string& code) const {
    auto it = codeToVertexMap.find(code);
    return (it != codeToVertexMap.end()) ? it->second : nullptr;
}

/*
 *  Adds a vertex with a given content or info (in) to a graph (this).
 *  Returns true if successful, and false if a vertex with that content already exists.
 */
template <class T>
bool Graph<T>::addVertex(const T &in, const std::string& code, bool parking) {
    if (idToVertexMap.find(in) != idToVertexMap.end()) return false;

    Vertex<T>* v = new Vertex<T>(in, code, parking);
    idToVertexMap[in] = v;
    codeToVertexMap[code] = v;
    return true;
}

/*
 *  Removes a vertex with a given content (in) from a graph (this), and
 *  all outgoing and incoming edges.
 *  Returns true if successful, and false if such vertex does not exist.
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

/*
 * Adds an edge to a graph (this), given the contents of the source and
 * destination vertices and the edge weight (w).
 * Returns true if successful, and false if the source or destination vertex does not exist.
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

/*
 * Removes an edge from a graph (this).
 * The edge is identified by the source (sourc) and destination (dest) contents.
 * Returns true if successful, and false if such edge does not exist.
 */
template <class T>
bool Graph<T>::removeEdge(const T &sourc, const T &dest) {
    Vertex<T> * srcVertex = findVertex(sourc);
    if (srcVertex == nullptr) {
        return false;
    }
    return srcVertex->removeEdge(dest);
}

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

template <class T>
Graph<T>::~Graph() {
    deleteMatrix(distMatrix, idToVertexMap.size());
    deleteMatrix(pathMatrix, idToVertexMap.size());
}


template <class T>
void Graph<T>::dijkstraDriving(const T& origin) {
    return; //TODO
}

#endif
