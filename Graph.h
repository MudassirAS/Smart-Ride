#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <string>
#include <algorithm> 
#include <climits>   

using namespace std;

class GraphEdge;

class GraphNode {
public:
    string data;
    vector<GraphEdge> edges;

    GraphNode(const string& data) : data(data) {}
};


class GraphEdge {
public:
    GraphNode* targetNode;
    int weight;

    GraphEdge(GraphNode* targetNode, int weight) : targetNode(targetNode), weight(weight) {}
};


class Graph {
public:
    vector<GraphNode*> nodes;

    GraphNode* getNode(const string& data) {
        for (auto node : nodes) 
            if (node->data == data)
                return node;

        return nullptr;
    }

    GraphEdge* getEdge(string& src, string& dest) {
        GraphNode* srcNode = getNode(src);
        GraphNode* destNode = getNode(dest);

        if (srcNode == nullptr || destNode == nullptr) 
            return nullptr;

        for (auto& edge : srcNode->edges) {
            if (edge.targetNode == destNode) 
                return &edge;
        }
        return nullptr; // Edge not found
    }

    int getEdgeWeight(string& src, string& dest) {
        GraphEdge* edge = getEdge(src, dest);
        if(edge != nullptr)
            return edge->weight;
        return 0;    
    }

    void addNode(const string& data) {
        if (getNode(data) != nullptr) {
            cout << "Node " << data << " already exists." << endl;
            return;
        }
        nodes.push_back(new GraphNode(data));
    }

    void addEdge(const string& data1, const string& data2, int weight) {
        GraphNode* node1 = getNode(data1);
        GraphNode* node2 = getNode(data2);

        if (node1 == nullptr || node2 == nullptr) {
            cout << "One or both of these nodes not found: " << data1 << ", " << data2 << endl;
            return;
        }

        node1->edges.push_back(GraphEdge(node2, weight));
        node2->edges.push_back(GraphEdge(node1, weight));
    }

    void removeNode(const string& data) {
        GraphNode* nodeToRemove = getNode(data);
        if (nodeToRemove == nullptr) {
            cout << "Node " << data << " not found." << endl;
            return;
        }

        for (auto node : nodes) {
            node->edges.erase(remove_if(node->edges.begin(), node->edges.end(), [nodeToRemove](GraphEdge& edge) {
                return edge.targetNode == nodeToRemove;
            }), node->edges.end());
        }

        nodes.erase(remove(nodes.begin(), nodes.end(), nodeToRemove), nodes.end());
        delete nodeToRemove;
    }

    void removeEdge(const string& data1, const string& data2) {
        GraphNode* node1 = getNode(data1);
        GraphNode* node2 = getNode(data2);

        if (node1 == nullptr || node2 == nullptr) {
            cout << "One or both nodes not found: " << data1 << ", " << data2 << endl;
            return;
        }

        node1->edges.erase(remove_if(node1->edges.begin(), node1->edges.end(), [node2](GraphEdge& edge) {
            return edge.targetNode == node2;
        }), node1->edges.end());

        node2->edges.erase(remove_if(node2->edges.begin(), node2->edges.end(), [node1](GraphEdge& edge) {
            return edge.targetNode == node1;
        }), node2->edges.end());
    }


    // Dijkstra's Algorithm to find shortest path from a source to a destination
    vector<string> findShortestPath(const string& source, const string& destination) {
        unordered_map<GraphNode*, int> distances; // Map to store distances from the source node
        unordered_map<string, GraphNode*> previousNodes; // Map to store the previous nodes in the shortest path
        auto compare = [&distances](GraphNode* a, GraphNode* b) {
            return distances[a] > distances[b]; // Compare distances for the priority queue
        };
        priority_queue<GraphNode*, vector<GraphNode*>, decltype(compare)> queue(compare);
        vector<string> path;

        GraphNode* start = getNode(source);
        GraphNode* end = getNode(destination);

        if (start == nullptr || end == nullptr) {
            cout << "One or both nodes not found: " << source << ", " << destination << endl;
            return path;
        }

        // Initialize distances with "infinity"
        for (auto node : nodes) {
            distances[node] = INT_MAX;
        }

        distances[start] = 0; // Distance to the start node is 0
        queue.push(start);

        while (!queue.empty()) {
            GraphNode* current = queue.top();
            queue.pop();

            if (current == end)
                break;

            for (auto& edge : current->edges) {
                int newDistance = distances[current] + edge.weight;
                if (newDistance < distances[edge.targetNode]) {
                    distances[edge.targetNode] = newDistance; // Update distance
                    previousNodes[edge.targetNode->data] = current; // Update previous node
                    queue.push(edge.targetNode);
                }
            }
        }

        // Construct the path from end to start
        for (GraphNode* at = end; at != nullptr; at = previousNodes[at->data]) 
            path.insert(path.begin(), at->data);

        if (path.empty() || path.front() != source) {
            cout << "No path found between " << source << " and " << destination << endl;
            return {};
        }

        return path;
    }

    int findDistance(const vector<string>& path) {
        int totalDistance = 0;
        for (size_t i = 0; i < path.size() - 1; ++i) {
            GraphNode* currentNode = getNode(path[i]);
            GraphNode* nextNode = getNode(path[i + 1]);

            if (currentNode == nullptr || nextNode == nullptr) {
                cout << "Invalid path." << endl;
                return -1;
            }

            for (auto& edge : currentNode->edges) {
                if (edge.targetNode == nextNode) {
                    totalDistance += edge.weight;
                    break;
                }
            }
        }

        return totalDistance;
    }

    void printGraph() {
        for (auto node : nodes) {
            cout << node->data << " -> ";
            for (auto& edge : node->edges) 
                cout << "(" << edge.targetNode->data << ", weight: " << edge.weight << ") ";
            
            cout << endl;
        }
    }

    void printPath(vector<string> path, string src, string dest){
        
        int count = 0 ,size = path.size();
      if (!path.empty()) {
        cout << "\nShortest path from " << src << " to " << dest << ": ";
        for (const auto& node : path)
            if(count++ + 1 == size)
                cout << node << " ";
            else
                cout << node << " -> ";
        } 
    }
    
    void displayLocations() {
        int i = 1;
        for (auto node : nodes) {
            cout << i << ". " << node->data << endl;
            ++i;
        }
    }

    void initializeGraph(Graph& graph) {
        vector<string> locationNames = {"Gate1", "HikingTrailStart", "NSHS", "NSTP", "SINES", "Gate10", "GirlsHostels",
                                                  "OldGym", "S3H", "NBS", "C1", "C3", "IGIS", "SADA", "JinnahAuditorium", "MainOffice",
                                                  "CentralLibrary", "ConvocationGround", "SEECS", "C2", "MasjidRehmat", "Admin",
                                                  "Gate15", "SCME", "IESE", "ASAB", "IqbalSquare", "Gate2", "BoysHostels", "Retro",
                                                  "USPCASE", "ExamHall", "Gate4", "NICE", "SNS", "SMME", "NCAI", "SportsComplex",
                                                  "NMC", "ResidentialArea", "IAEC", "NCLS"};


        sort(locationNames.begin(), locationNames.end());

        for (const auto& name : locationNames) 
            graph.addNode(name);

        graph.addEdge("Gate1", "HikingTrailStart", 150);
        graph.addEdge("HikingTrailStart", "NSHS", 300);
        graph.addEdge("HikingTrailStart", "NSTP", 530);
        graph.addEdge("HikingTrailStart", "GirlsHostels", 850);
        graph.addEdge("NSHS", "NSTP", 350);
        graph.addEdge("NSHS", "OldGym", 650);
        graph.addEdge("NSTP", "SINES", 50);
        graph.addEdge("Gate10", "NMC", 320);
        graph.addEdge("NMC", "ResidentialArea", 250);
        graph.addEdge("ResidentialArea", "SportsComplex", 600);
        graph.addEdge("SportsComplex", "SNS", 650);
        graph.addEdge("SNS", "SMME", 80);
        graph.addEdge("SNS", "NCAI", 100);
        graph.addEdge("SMME", "NCAI", 200);
        graph.addEdge("SNS", "Retro", 400);
        graph.addEdge("Retro", "BoysHostels", 200);
        graph.addEdge("BoysHostels", "Gate15", 180);
        graph.addEdge("Gate15", "USPCASE", 110);
        graph.addEdge("USPCASE", "NICE", 190);
        graph.addEdge("USPCASE", "ExamHall", 140);
        graph.addEdge("NICE", "ExamHall", 80);
        graph.addEdge("NICE", "Gate4", 130);
        graph.addEdge("ExamHall", "Gate4", 40);
        graph.addEdge("NSTP", "OldGym", 680);
        graph.addEdge("NSTP", "GirlsHostels", 750);
        graph.addEdge("OldGym", "GirlsHostels", 60);
        graph.addEdge("GirlsHostels", "MainOffice", 130);
        graph.addEdge("Gate10", "NSTP", 700);
        graph.addEdge("NSTP", "NCLS", 1000);
        graph.addEdge("Gate10", "NCLS", 1200);
        graph.addEdge("Gate10", "GirlsHostels", 1200);
        graph.addEdge("Gate10", "JinnahAuditorium", 1080);
        graph.addEdge("JinnahAuditorium", "MainOffice", 40);
        graph.addEdge("MainOffice", "CentralLibrary", 60);
        graph.addEdge("IqbalSquare", "ConvocationGround", 680);
        graph.addEdge("MainOffice", "Gate10", 1250);
        graph.addEdge("CentralLibrary", "C3", 90);
        graph.addEdge("C2", "C3", 300);
        graph.addEdge("SEECS", "C2", 170);
        graph.addEdge("SEECS", "IAEC", 230);
        graph.addEdge("C2", "IAEC", 160);
        graph.addEdge("IAEC", "MasjidRehmat", 80);
        graph.addEdge("MasjidRehmat", "Admin", 110);
        graph.addEdge("Admin", "IqbalSquare", 170);
        graph.addEdge("IqbalSquare", "Gate2", 270);
        graph.addEdge("Admin", "Gate15", 250);
        graph.addEdge("C2", "Gate15", 250);
        graph.addEdge("BoysHostels", "Gate15", 170);
        graph.addEdge("OldGym", "S3H", 50);
        graph.addEdge("OldGym", "C1", 230);
        graph.addEdge("OldGym", "SCME", 380);
        graph.addEdge("OldGym", "IESE", 260);
        graph.addEdge("SCME", "IESE", 280);
        graph.addEdge("SCME", "C1", 260);
        graph.addEdge("IESE", "C1", 80);
        graph.addEdge("C1", "SADA", 110);
        graph.addEdge("SADA", "ASAB", 90);
        graph.addEdge("SADA", "IGIS", 150);
        graph.addEdge("IESE", "ASAB", 240);
        graph.addEdge("ASAB", "IqbalSquare", 190);
        graph.addEdge("NBS", "S3H", 150);
        graph.addEdge("NBS", "C1", 200);
    }

    void getRoute(Graph map, string source , string destination) {
        
        map.initializeGraph(map);
    
        vector<string> shortestPath = map.findShortestPath(source, destination);
        int size = shortestPath.size() , count = 0;

        if (!shortestPath.empty()) {
            cout << "Shortest path from " << source << " to " << destination << ": ";
            
            for (const auto& node : shortestPath) {
                if(++count == size)
                    cout << node << " ";
                
                else
                cout << node << " -> ";
            }
            cout << endl;

            int distance = map.findDistance(shortestPath);
            cout << "\nTotal distance: " << distance << " meters" << endl;
        }
    }

    string getLocationFromIndex(int index) {
        return nodes[index - 1]->data;
    }

    int sizeofMap(){
        return nodes.size();
    }
};


#endif