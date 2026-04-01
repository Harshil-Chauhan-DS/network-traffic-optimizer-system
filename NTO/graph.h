/*
============================================================
  graph.h  --  Shared Data Layer
  Network Traffic Optimizer  |  All Members
============================================================
  Include in every module:   #include "graph.h"

  Contains:
    - Print helpers (ASCII only)
    - Constants
    - Edge struct
    - Packet struct        (Member 2)
    - RouteEntry struct    (Member 1 + 4)
    - DSU struct           (Member 1 - Kruskal)
    - Graph class
    - buildTestNetwork()   -- 12-node shared topology
    - buildTestPackets()   -- 20 packets for Member 2
    - buildNetwork()       -- dynamic input dispatcher
============================================================
*/

#pragma once
#include <bits/stdc++.h>
using namespace std;

// ============================================================
//  CONSTANTS
// ============================================================
const int INF = INT_MAX / 2;

// ============================================================
//  ASCII PRINT HELPERS
// ============================================================
inline void printHeader(const string& title) {
    int width = 56;
    string line(width, '=');
    int pad = max(0, (int)(width - 2 - title.size()));
    cout << "\n+" << line << "+\n"
         << "| " << title << string(pad, ' ') << " |\n"
         << "+" << line << "+\n";
}

inline void printSubHeader(const string& s) {
    cout << "\n--- " << s << " ---\n";
}

inline void printSep() {
    cout << string(60, '-') << "\n";
}

inline void printExplain(const string& algo, const string& desc,
                          const string& complexity) {
    cout << "\n[" << algo << "]\n";
    cout << "  What it does : " << desc << "\n";
    cout << "  Complexity   : " << complexity << "\n";
    cout << "  Running now...\n";
    printSep();
}

// ============================================================
//  EDGE
// ============================================================
struct Edge {
    int u, v, weight;
    bool operator<(const Edge& o) const { return weight < o.weight; }
};

// ============================================================
//  PACKET  (Member 2 -- sorting / scheduling)
//  priority: 1 = most urgent, 5 = background
//  deadline: latest delivery time in ms
// ============================================================
struct Packet {
    int    id;
    int    priority;
    int    size;         // KB
    int    deadline;     // ms
    int    arrivalTime;  // ms
    int    srcNode;
    int    dstNode;
    string protocol;     // TCP / UDP / ICMP

    bool operator<(const Packet& o) const {
        if (priority != o.priority) return priority < o.priority;
        return deadline < o.deadline;
    }
};

inline void printPacketHeader() {
    cout << "  " << left
         << setw(5)  << "ID"
         << setw(6)  << "Pri"
         << setw(10) << "Size(KB)"
         << setw(12) << "Deadline"
         << setw(12) << "Arrival"
         << setw(8)  << "Proto"
         << setw(6)  << "Src"
         << "Dst\n";
    printSep();
}

inline void printPacketRow(const Packet& p) {
    cout << "  " << left
         << setw(5)  << p.id
         << setw(6)  << p.priority
         << setw(10) << p.size
         << setw(12) << p.deadline
         << setw(12) << p.arrivalTime
         << setw(8)  << p.protocol
         << setw(6)  << ("R" + to_string(p.srcNode))
         << "R" + to_string(p.dstNode) << "\n";
}

// ============================================================
//  ROUTE ENTRY  (Member 1 + 4)
// ============================================================
struct RouteEntry {
    int destination;
    int nextHop;
    int cost;
    bool operator<(const RouteEntry& o) const {
        return destination < o.destination;
    }
};

// ============================================================
//  DSU -- Disjoint Set Union  (Member 1 - Kruskal)
// ============================================================
struct DSU {
    vector<int> parent, rank_;
    DSU(int n) : parent(n), rank_(n, 0) {
        iota(parent.begin(), parent.end(), 0);
    }
    int find(int x) {
        if (parent[x] != x) parent[x] = find(parent[x]);
        return parent[x];
    }
    bool unite(int x, int y) {
        int px = find(x), py = find(y);
        if (px == py) return false;
        if (rank_[px] < rank_[py]) swap(px, py);
        parent[py] = px;
        if (rank_[px] == rank_[py]) rank_[px]++;
        return true;
    }
    bool connected(int x, int y) { return find(x) == find(y); }
};

// ============================================================
//  GRAPH CLASS
// ============================================================
class Graph {
public:
    int V, E;
    vector<string>               nodeNames;
    vector<vector<pair<int,int>>> adj;    // forward  adj[u] = {v, w}
    vector<vector<pair<int,int>>> radj;   // reverse  radj[v] = {u, w}
    vector<Edge>                 edges;   // flat directed list

    Graph(int v, const vector<string>& names)
        : V(v), E(0), nodeNames(names), adj(v), radj(v) {}

    // Directed edge  u -> v
    void addDirectedEdge(int u, int v, int w) {
        adj[u].push_back({v, w});
        radj[v].push_back({u, w});
        edges.push_back({u, v, w});
        ++E;
    }

    // Undirected edge  u <-> v
    void addUndirectedEdge(int u, int v, int w) {
        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
        radj[v].push_back({u, w});
        radj[u].push_back({v, w});
        edges.push_back({u, v, w});
        edges.push_back({v, u, w});
        E += 2;
    }

    const string& name(int i) const { return nodeNames[i]; }

    int indexOf(const string& n) const {
        for (int i = 0; i < V; ++i)
            if (nodeNames[i] == n) return i;
        return -1;
    }

    bool hasEdge(int u, int v) const {
        for (auto [nb, w] : adj[u]) if (nb == v) return true;
        return false;
    }

    int getWeight(int u, int v) const {
        for (auto [nb, w] : adj[u]) if (nb == v) return w;
        return INF;
    }

    // Deduplicated undirected edges for Kruskal
    vector<Edge> getUndirectedEdges() const {
        map<pair<int,int>, int> best;
        for (auto& e : edges) {
            auto key = make_pair(min(e.u,e.v), max(e.u,e.v));
            if (!best.count(key) || e.weight < best[key])
                best[key] = e.weight;
        }
        vector<Edge> res;
        for (auto& [k,w] : best)
            res.push_back({k.first, k.second, w});
        return res;
    }

    void printAdjList() const {
        printSubHeader("Adjacency List (" + to_string(V)
                       + " nodes, " + to_string(E) + " edges)");
        for (int u = 0; u < V; ++u) {
            cout << "  " << left << setw(6) << name(u) << "-> ";
            if (adj[u].empty()) { cout << "(none)\n"; continue; }
            for (auto [v,w] : adj[u])
                cout << name(v) << "(" << w << "ms)  ";
            cout << "\n";
        }
    }

    void printSummary() const {
        cout << "\n  Nodes          : " << V << "\n";
        cout << "  Directed edges : " << E << "\n";
        int minW = INF, maxW = 0, sumW = 0;
        for (auto& e : edges) {
            minW = min(minW, e.weight);
            maxW = max(maxW, e.weight);
            sumW += e.weight;
        }
        if (!edges.empty()) {
            cout << "  Weight range   : " << minW << "ms - " << maxW << "ms\n";
            cout << fixed << setprecision(1);
            cout << "  Avg weight     : " << (double)sumW/edges.size() << "ms\n";
        }
    }
};

// ============================================================
//  buildTestNetwork()
//  12 routers (R0-R11), 19 directed edges, latency in ms.
//  Used by ALL four members.
// ============================================================
inline Graph buildTestNetwork() {
    Graph g(12, {"R0","R1","R2","R3","R4","R5",
                 "R6","R7","R8","R9","R10","R11"});
    g.addDirectedEdge(0,  1,  4);
    g.addDirectedEdge(1,  2,  8);
    g.addDirectedEdge(0,  3, 11);
    g.addDirectedEdge(1,  3,  5);
    g.addDirectedEdge(2,  5,  2);
    g.addDirectedEdge(2,  4,  3);
    g.addDirectedEdge(3,  4,  7);
    g.addDirectedEdge(3,  6,  6);
    g.addDirectedEdge(4,  5,  9);
    g.addDirectedEdge(4,  7, 14);
    g.addDirectedEdge(5,  8,  7);
    g.addDirectedEdge(5, 10,  9);
    g.addDirectedEdge(6,  7,  3);
    g.addDirectedEdge(6,  9,  8);
    g.addDirectedEdge(7,  8,  5);
    g.addDirectedEdge(7,  9,  2);
    g.addDirectedEdge(8, 11,  3);
    g.addDirectedEdge(9, 10,  4);
    g.addDirectedEdge(10,11,  6);
    return g;
}

// ============================================================
//  buildTestPackets()  -- 20 packets for Member 2
// ============================================================
inline vector<Packet> buildTestPackets() {
    return {
        { 1,1, 512, 50, 0,0,11,"TCP" }, { 2,3, 128,200,10,1, 8,"UDP" },
        { 3,2, 256,100, 5,0, 5,"TCP" }, { 4,5,1024,800,30,3,10,"UDP" },
        { 5,1,  64, 40, 2,0,11,"ICMP"},  { 6,4, 768,500,25,2, 7,"TCP" },
        { 7,2, 192, 90, 8,1, 9,"TCP" }, { 8,3, 320,220,15,4, 6,"UDP" },
        { 9,1,  96, 60, 1,0, 8,"TCP" }, {10,5,2048,900,40,5,11,"UDP" },
        {11,2, 160,110,12,0, 4,"TCP" }, {12,4, 448,420,20,3, 9,"UDP" },
        {13,1,  80, 45, 3,0,11,"ICMP"}, {14,3, 288,250,18,2, 7,"TCP" },
        {15,2, 224,130,10,1, 6,"UDP" }, {16,5, 896,850,35,4,10,"TCP" },
        {17,1, 112, 55, 4,0, 5,"TCP" }, {18,3, 352,300,22,3, 8,"UDP" },
        {19,4, 576,460,28,2,11,"TCP" }, {20,2, 208,120, 9,1, 7,"TCP" },
    };
}

// ============================================================
//  DYNAMIC INPUT -- File / Terminal / Demo
//
//  File format (network.txt):
//    Line 1 : V E
//    Line 2 : name0 name1 ... nameV-1
//    Lines  : u v w   (one directed edge per line)
// ============================================================
inline Graph loadFromFile(const string& filename) {
    ifstream fin(filename);
    if (!fin.is_open()) {
        cout << "[ERROR] Cannot open: " << filename
             << "  -- falling back to demo network.\n";
        return Graph(0, {});
    }
    int V, E;
    if (!(fin >> V >> E) || V <= 0) {
        cout << "[ERROR] Bad header line.\n";
        return Graph(0, {});
    }
    vector<string> names(V);
    for (int i = 0; i < V; ++i) fin >> names[i];
    Graph g(V, names);
    for (int i = 0; i < E; ++i) {
        int u, v, w;
        if (!(fin >> u >> v >> w) || u < 0 || u >= V || v < 0 || v >= V) {
            cout << "[ERROR] Bad edge at line " << i+3 << "\n";
            return Graph(0, {});
        }
        g.addDirectedEdge(u, v, w);
    }
    cout << "[OK] Loaded " << filename << " -- "
         << V << " nodes, " << g.E << " edges.\n";
    return g;
}

inline Graph loadFromTerminal() {
    cout << "\nEnter number of routers (nodes): ";
    int V; cin >> V;
    cout << "Enter " << V << " node names (space-separated): ";
    vector<string> names(V);
    for (int i = 0; i < V; ++i) cin >> names[i];
    Graph g(V, names);
    cout << "Enter number of directed edges: ";
    int E; cin >> E;
    cout << "Enter each edge as: from_index  to_index  weight_ms\n";
    cout << "(Node indices are 0 to " << V-1 << ")\n\n";
    for (int i = 0; i < E; ++i) {
        int u, v, w;
        cout << "  Edge " << i+1 << "/" << E << " > ";
        cin >> u >> v >> w;
        if (u < 0 || u >= V || v < 0 || v >= V) {
            cout << "[WARN] Index out of range, skipping.\n"; --i; continue;
        }
        g.addDirectedEdge(u, v, w);
    }
    cout << "[OK] Network ready -- " << V << " nodes, " << g.E << " edges.\n";
    return g;
}

inline Graph buildNetwork() {
    printHeader("Network Traffic Optimizer -- Input Mode");
    cout << "\n  1  Use built-in 12-node demo network\n"
         << "  2  Load from file  (e.g. network.txt)\n"
         << "  3  Enter manually via terminal\n\n"
         << "  Choice [1/2/3]: ";
    int c; cin >> c;
    if (c == 2) {
        cout << "  Filename: ";
        string f; cin >> f;
        Graph g = loadFromFile(f);
        if (g.V > 0) return g;
    }
    if (c == 3) return loadFromTerminal();
    cout << "\n  Loading demo network...\n";
    return buildTestNetwork();
}
