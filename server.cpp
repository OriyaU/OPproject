#include <arpa/inet.h>  // For inet_ntop
#include <netdb.h>      // For addrinfo
#include <netinet/in.h> // For sockaddr_in and sockaddr_in6
#include <sys/socket.h> // For socket functions
#include <unistd.h>     // For close

#include <algorithm>
#include <cassert>
#include <csignal> // For signal handling
#include <cstring> // For memset
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <numeric>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "graph.hpp"
#include "LeaderFollowerPool.hpp"
#include "MST_factory.hpp"
#include "MST.hpp"
#include "MSTTree.hpp"
#include "MST_strategy.hpp"
#include "Pipeline.hpp"
#include "unionfind.hpp"

#define PORT "9034"   // Port for the server
#define NUM_THREADS 4 // Number of threads in the thread pool

using namespace std;

// Global Variables
unordered_map<int, tuple<Graph, MST, string>> map_clients; // Per-client data
std::mutex lfMutex;                                        // Mutex for thread safety
LeaderFollowerPool threadPool(NUM_THREADS, lfMutex);       // Leader-Follower Thread Pool
Pipeline pipeline;                                         // Task pipeline
MST_strategy mst;
MST_factory factory;
atomic<bool> isMST{false};

int listener; // Global listener socket for shutdown handling

// Signal handler for server shutdown
void shutdown_handler(int signum) {
    (void)signum; // מבהיר למהדר שהפרמטר לא בשימוש בכוונה
    //std::cout << "\n[Server] Shutting down..." << std::endl;
    close(listener); // Close the listener socket
    exit(0);         // Exit gracefully
}

// Helper to parse and validate graph creation input
Graph Newgraph(istringstream &iss) {
    int numVertices, numEdges;
    iss >> numVertices >> numEdges;

    if (numVertices <= 0 || numEdges < 0) {
        throw std::invalid_argument("[Error] Invalid number of vertices or edges.");
    }

    Graph graph(numVertices);

    // Add edges to the graph
    int u, v, w;
    for (int i = 0; i < numEdges; ++i) {
        if (!(iss >> u >> v >> w)) {
            throw std::invalid_argument("[Error] Insufficient edge data provided.");
        }
        if (u < 0 || u >= numVertices || v < 0 || v >= numVertices) {
            throw std::out_of_range("[Error] Vertex index out of range.");
        }
        graph.add_edge(u, v, w);
    }

    return graph;
}

// Process commands received from the client
string process_user_command(string input_user, Graph &clientGraph, MST &clientMST, string &clientAns, int fdclient) {
    try {
        istringstream iss(input_user);
        string command;
        iss >> command;

        if (command == "Newgraph") {
            clientGraph = Newgraph(iss);
            return "[Server] Graph created successfully.\n";
        } else if (command == "kruskal" || command == "prim") {
            auto pclientMST = factory.createMST(clientGraph.get_adj_matrix(), clientGraph.get_adj_matrix().size(), command);
            clientMST = *pclientMST;
            clientMST.setStrategy(command);
            delete pclientMST;
           return "[Server] MST created using " + command + " algorithm.\n";
        } else if (command == "Newedge") {
            int from, to, weight;
            if (!(iss >> from >> to >> weight)) {
                throw std::invalid_argument("[Error] Invalid edge format.");
            }
            clientGraph.add_edge(from, to, weight);
           
            
            return "[Server] Edge added from " + to_string(from) + " to " + to_string(to) + ".\n";
        } else if (command == "Removeedge") {
            int from, to;
            if (!(iss >> from >> to)) {
                throw std::invalid_argument("[Error] Invalid edge format.");
            }
            clientGraph.remove_edge(from, to);
            return "[Server] Edge removed from " + to_string(from) + " to " + to_string(to) + ".\n";
        } else if (command == "Leader_Follower") {
            auto *clientTask = new std::tuple<MST *, string *, int>(&clientMST, &clientAns, fdclient);
            threadPool.addEventHandler(clientTask);
          //  return "[Server] Leader-Follower task queued.\n";
        } else if (command == "Pipeline") {
            auto *clientTask = new std::tuple<MST *, string *, int>(&clientMST, &clientAns, fdclient);
            pipeline.addRequest(clientTask);
            return "[Server] Pipeline task queued.\n";
        } else {
            return "[Error] Unknown command: " + command + "\n";
        }
    } catch (const std::exception &e) {
        return string("[Error] ") + e.what() + "\n";
    }
    // Default return statement for unexpected scenarios
    return "[Error] Unknown error occurred.\n";
}

int main() {
    fd_set master;                         // Master file descriptor list
    fd_set read_fds;                       // Temp file descriptor list
    int fdmax;                             // Maximum file descriptor number
    threadPool.start();                    // Start Leader-Follower Pool
    pipeline.start(lfMutex);               // Start Pipeline
    int newfd;                             // New accepted socket descriptor
    struct sockaddr_storage clientAddress; // Client address
    socklen_t addrlen;
    char buf[4096]; // Buffer for client data
    int nbytes;
    int yes = 1;
    int rv;
    struct addrinfo hints, *ai, *p;

    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    signal(SIGINT, shutdown_handler); // Set up shutdown handler

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        cerr << "[Server] getaddrinfo error: " << gai_strerror(rv) << endl;
        return 1;
    }

    for (p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) continue;
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }
        break;
    }

    if (p == NULL) {
        cerr << "[Server] Failed to bind socket." << endl;
        return 2;
    }

    freeaddrinfo(ai);

    if (listen(listener, 10) == -1) {
        perror("[Server] listen");
        return 3;
    }

    FD_SET(listener, &master);
    fdmax = listener;

    cout << "[Server] Listening on port " << PORT << endl;

    for (;;) {
        read_fds = master;
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("[Server] select");
            return 4;
        }

        for (int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == listener) {
                    addrlen = sizeof clientAddress;
                    newfd = accept(listener, (struct sockaddr *)&clientAddress, &addrlen);
                    if (newfd == -1) {
                        perror("[Server] accept");
                    } else {
                        FD_SET(newfd, &master);
                        if (newfd > fdmax) fdmax = newfd;
                        map_clients[newfd] = make_tuple(Graph(), MST(), "");
                      // cout << "[Server] New connection on socket " << newfd << endl;
                    }
                } else {
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                        if (nbytes == 0) cout << "[Server] Socket " << i << " disconnected." << endl;
                        else perror("[Server] recv");
                        close(i);
                        FD_CLR(i, &master);
                        map_clients.erase(i);
                    } else {
                        buf[nbytes] = '\0';
                        string response = process_user_command(string(buf), get<0>(map_clients[i]), get<1>(map_clients[i]), get<2>(map_clients[i]), i);
                        if (send(i, response.c_str(), response.length(), 0) == -1) perror("[Server] send");
                    }
                }
            }
        }
    }

    return 0;
}
