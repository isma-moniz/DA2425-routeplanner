#ifndef GRAPH_H
#define GRAPH_H

#include <vector>

template <class T>
class Edge;

#define INF std::numeric_limits<double>::max()

/******************** Vertex ********************/

template <class T>
class Vertex {
public:
    Vertex(T in); // should this be the code or the id?
    bool operator<(Vertex<T>& vertex) const; // comparison operator overload
    
    T getInfo() const;
    std::vector<Edge<T> *> getAdj() const;
    bool isVisited() const;
    bool isProcessing() const;
    unsigned int getIndegree() const;
    double getDist() const;
    Edge<T>* getPath() const;
    std::vector<Edge<T>*> getIncoming() const;

    void setInfo(T info);
    void setVisited(bool visited);
    void setProcessing(bool processing);

    int getLow() const;
    void setLow(int value);
    int getNum() const;
    void setNum(int value);

    void setIndegree(unsigned int indegree);
    void setDist(double dist);
    void setPath(Edge<T>* path);
    Edge<T>* addEdge(Vertex<T>* dest, double w);
};

#endif
