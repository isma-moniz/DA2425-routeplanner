/**
 * @file graph.hpp
 * @brief Graph data structure implementation.
 * @details This file contains the implementation of a graph using vertices and edges.
 */

 #include "graph.hpp"

 /************************* Vertex  **************************/
 
 /**
  * @brief Constructs a Vertex.
  * @param in The vertex information.
  * @param code The unique identifier code for the vertex.
  * @param parking Indicates whether the vertex has a parking space.
  */
 template <class T>
 Vertex<T>::Vertex(T in, const std::string& code, bool parking): info(in), code(code), parkingSpace(parking) {}
 
 /**
  * @brief Adds an outgoing edge to a vertex.
  * @param dest The destination vertex.
  * @param w The walking time weight.
  * @param d The driving time weight.
  * @return The created edge.
  * 
  * complexity O(1)
  */
 template <class T>
 Edge<T> * Vertex<T>::addEdge(Vertex<T> *dest, double w, double d) {
     auto newEdge = new Edge<T>(this, dest, w, d);
     adj.push_back(newEdge);
     dest->incoming.push_back(newEdge);
     return newEdge;
 }
 
 /**
  * @brief Removes an outgoing edge with a given destination.
  * @param in The information of the destination vertex.
  * @return True if the edge was removed, false otherwise.
  * 
  * complexity O(E) where E is the number of edges.
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
             removedEdge = true;
         }
         else {
             it++;
         }
     }
     return removedEdge;
 }
 
 /**
  * @brief Removes all outgoing edges of a vertex.
  * 
  * complexity O(E) where E is the number of edges.
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
  * @param vertex The vertex to compare against.
  * @return True if this vertex has a smaller distance.
  * 
  * complexity O(1)
  */
 template <class T>
 bool Vertex<T>::operator<(Vertex<T>& vertex) const {
     return this->dist < vertex.dist;
 }
 
 /**
  * @brief Retrieves the information stored in the vertex.
  * @return The information of the vertex.
  * 
  * complexity O(1)
  */
 template <class T>
 T Vertex<T>::getInfo() const {
     return this->info;
 }
 
 /**
  * @brief Retrieves the code of the vertex.
  * @return The code of the vertex.
  * 
  * complexity O(1)
  */
 template <class T>
 const std::string& Vertex<T>::getCode() const {
     return this->code;
 }
 
 /**
  * @brief Sets the code of the vertex.
  * @param code The new code to set.
  * 
  * complexity O(1)
  */
 template <class T>
 void Vertex<T>::setCode(const std::string& code) {
     this->code = code;
 }
 
 /**
  * @brief Retrieves the low value of the vertex.
  * @return The low value.
  * 
  * complexity O(1)
  */
 template <class T>
 int Vertex<T>::getLow() const {
     return this->low;
 }
 
 /**
  * @brief Sets the low value of the vertex.
  * @param value The new low value.
  * 
  * complexity O(1)
  */
 template <class T>
 void Vertex<T>::setLow(int value) {
     this->low = value;
 }
 
 /**
  * @brief Retrieves the num value of the vertex.
  * @return The num value.
  * 
  * complexity O(1)
  */
 template <class T>
 int Vertex<T>::getNum() const {
     return this->num;
 }
 
 /**
  * @brief Sets the num value of the vertex.
  * @param value The new num value.
  * 
  * complexity O(1)
  */
 template <class T>
 void Vertex<T>::setNum(int value) {
     this->num = value;
 }
 
 /**
  * @brief Retrieves the adjacency list of the vertex.
  * @return The adjacency list of the vertex.
  * 
  * complexity O(1)
  */
 template <class T>
 std::vector<Edge<T>*> Vertex<T>::getAdj() const {
     return this->adj;
 }
 
 /**
  * @brief Checks if the vertex is visited.
  * @return True if the vertex is visited, false otherwise.
  * 
  * complexity O(1)
  */
 template <class T>
 bool Vertex<T>::isVisited() const {
     return this->visited;
 }
 
 /**
  * @brief Checks if the vertex is in processing state.
  * @return True if the vertex is being processed, false otherwise.
  * 
  * complexity O(1)
  */
 template <class T>
 bool Vertex<T>::isProcessing() const {
     return this->processing;
 }
 
 /**
  * @brief Checks if the vertex has a parking space.
  * @return True if the vertex has a parking space, false otherwise.
  * 
  * complexity O(1)
  */
 template <class T>
 bool Vertex<T>::hasParking() const {
     return this->parkingSpace;
 }
 
 /**
  * @brief Retrieves the indegree of the vertex.
  * @return The indegree of the vertex.
  * 
  * complexity O(1)
  */
 template <class T>
 unsigned int Vertex<T>::getIndegree() const {
     return this->indegree;
 }
 
 /**
  * @brief Retrieves the distance of the vertex.
  * @return The distance value.
  * 
  * complexity O(1)
  */
 template <class T>
 double Vertex<T>::getDist() const {
     return this->dist;
 }
 
 /**
  * @brief Retrieves the edge representing the path to the vertex.
  * @return The edge representing the path.
  * 
  * complexity O(1)
  */
 template <class T>
 Edge<T> *Vertex<T>::getPath() const {
     return this->path;
 }
 
 /**
  * @brief Retrieves the incoming edges of the vertex.
  * @return A vector of incoming edges.
  * 
  * complexity O(1)
  */
 template <class T>
 std::vector<Edge<T> *> Vertex<T>::getIncoming() const {
     return this->incoming;
 }
 
 /**
  * @brief Sets the information stored in the vertex.
  * @param in The new information for the vertex.
  * 
  * complexity O(1)
  */
 template <class T>
 void Vertex<T>::setInfo(T in) {
     this->info = in;
 }
 
 /**
  * @brief Sets the visited state of the vertex.
  * @param visited The visited state to set.
  * 
  * complexity O(1)
  */
 template <class T>
 void Vertex<T>::setVisited(bool visited) {
     this->visited = visited;
 }
 
 /** 
  * @brief Sets the processing state of the vertex.
  * @param processing The processing state to set.
  * 
  * complexity O(1)
  */
 template <class T>
 void Vertex<T>::setProcessing(bool processing) {
     this->processing = processing;
 }
 
 /**
  * @brief Sets whether the vertex has a parking space.
  * @param parking The parking state to set.
  * 
  * complexity O(1)
  */
 template <class T>
 void Vertex<T>::setParking(bool parking) {
     this->parkingSpace = parking;
 }
 
 /**
  * @brief Sets the indegree of the vertex.
  * @param indegree The new indegree value.
  * 
  * complexity O(1)
  */
 template <class T>
 void Vertex<T>::setIndegree(unsigned int indegree) {
     this->indegree = indegree;
 }
 
 /**
  * @brief Sets the distance value of the vertex.
  * @param dist The new distance value.
  * 
  * complexity O(1)
  */
 template <class T>
 void Vertex<T>::setDist(double dist) {
     this->dist = dist;
 }
 
 /**
  * @brief Sets the path to the vertex as an edge.
  * @param path The edge representing the path.
  * 
  * complexity O(1)
  */
 template <class T>
 void Vertex<T>::setPath(Edge<T> *path) {
     this->path = path;
 }
 
 /**
  * @brief Deletes an edge and removes it from the incoming list of the destination vertex.
  * @param edge The edge to be deleted.
  * 
  * complexity O(E)
  */
 template <class T>
 void Vertex<T>::deleteEdge(Edge<T> *edge) {
     Vertex<T> *dest = edge->getDest();
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
  * @brief Constructs an edge.
  * @param orig The origin vertex.
  * @param dest The destination vertex.
  * @param w The walking time weight.
  * @param d The driving time weight.
  */
 template <class T>
 Edge<T>::Edge(Vertex<T> *orig, Vertex<T> *dest, double w, double d): origin(orig), dest(dest), walkTime(w), driveTime(d) {}
 
 /**
  * @brief Retrieves the destination vertex of the edge.
  * @return The destination vertex.
  * 
  * complexity O(1)
  */
 template <class T>
 Vertex<T> * Edge<T>::getDest() const {
     return this->dest;
 }
 
 /**
  * @brief Retrieves the driving time weight of the edge.
  * @return The driving time weight.
  * 
  * complexity O(1)
  */
 template <class T>
 double Edge<T>::getDriveTime() const {
     return this->driveTime;
 }
 
 /**
  * @brief Retrieves the walking time weight of the edge.
  * @return The walking time weight.
  * 
  * complexity O(1)
  */
 template <class T>
 double Edge<T>::getWalkTime() const {
     return this->walkTime;
 }
 
 /**
  * @brief Retrieves the origin vertex of the edge.
  * @return The origin vertex.
  * 
  * complexity O(1)
  */
 template <class T>
 Vertex<T> * Edge<T>::getOrigin() const {
     return this->origin;
 }
 
 /**
  * @brief Retrieves the reverse edge.
  * @return The reverse edge.
  * 
  * complexity O(1)
  */
 template <class T>
 Edge<T> *Edge<T>::getReverse() const {
     return this->reverse;
 }
 
 /**
  * @brief Checks if the edge is selected.
  * @return True if the edge is selected, false otherwise.
  * 
  * complexity O(1)
  */
 template <class T>
 bool Edge<T>::isSelected() const {
     return this->selected;
 }
 
 /**
  * @brief Retrieves the flow value of the edge.
  * @return The flow value.
  * 
  * complexity O(1)
  */
 template <class T>
 double Edge<T>::getFlow() const {
     return flow;
 }
 
 /**
  * @brief Sets the selected state of the edge.
  * @param selected The selected state to set.
  * 
  * complexity O(1)
  */
 template <class T>
 void Edge<T>::setSelected(bool selected) {
     this->selected = selected;
 }
 
 /**
  * @brief Sets the reverse edge.
  * @param reverse The reverse edge to set.
  * 
  * complexity O(1)
  */
 template <class T>
 void Edge<T>::setReverse(Edge<T> *reverse) {
     this->reverse = reverse;
 }
 
 /**
  * @brief Sets the flow value of the edge.
  * @param flow The flow value to set.
  * 
  * complexity O(1)
  */
 template <class T>
 void Edge<T>::setFlow(double flow) {
     this->flow = flow;
 }
 
 /********************** Graph  ****************************/
 
 /**
  * @brief Retrieves the number of vertices in the graph.
  * @return The number of vertices.
  * 
  * complexity O(1)
  */
 template <class T>
 int Graph<T>::getNumVertex() const {
     return idToVertexMap.size(); 
 }
 
 /**
  * @brief Retrieves the set of vertices in the graph.
  * @return A vector of vertices.
  * 
  * complexity O(V) where V is the number of vertices.
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
  * @brief Finds a vertex by its information.
  * @param in The information of the vertex.
  * @return A pointer to the vertex if found, nullptr otherwise.
  * 
  * complexity O(1)
  */
 template <class T>
 Vertex<T>* Graph<T>::findVertex(const T &in) const {
     auto it = idToVertexMap.find(in);
     return (it != idToVertexMap.end()) ? it->second : nullptr;
 }
 
 /**
  * @brief Adds a vertex to the graph.
  * @param in The vertex information.
  * @param code The unique identifier code for the vertex.
  * @param parking Indicates whether the vertex has a parking space.
  * @return True if added successfully, false if vertex already exists.
  * 
  * complexity O(1)
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
  * @brief Removes a vertex from the graph.
  * @param in The information of the vertex.
  * @return True if removed successfully, false otherwise.
  * 
  * complexity O(V + E) where V is the number of vertices and E is edges.
  */
 template <class T>
 bool Graph<T>::removeVertex(const T &in) {
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
 
 /**
  * @brief Destructor for Graph.
  */
 template <class T>
 Graph<T>::~Graph() {
     deleteMatrix(distMatrix, idToVertexMap.size());
     deleteMatrix(pathMatrix, idToVertexMap.size());
 }
 