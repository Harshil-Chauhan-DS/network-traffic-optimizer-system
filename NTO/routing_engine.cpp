/*
============================================================
  routing_engine.cpp  --  Member 1  |  Units 1 + 3
  Network Traffic Optimizer
============================================================
  Compile:
    g++ -std=c++11 -O2 -o routing_engine routing_engine.cpp
  Run:
    ./routing_engine

  Algorithms (only those relevant to traffic routing):
    1. Binary Search     -- O(log V)        Unit 1
    2. BFS               -- O(V+E)          Unit 3
    3. DFS               -- O(V+E)          Unit 3
    4. Cycle Detection   -- O(V+E)          Unit 3
    5. Topological Sort  -- O(V+E)          Unit 3
    6. Dijkstra          -- O((V+E) log V)  Unit 3
    7. Bellman-Ford      -- O(V*E)          Unit 3
    8. Floyd-Warshall    -- O(V^3)          Unit 3
    9. Kruskal MST       -- O(E log E)      Unit 3
   10. Prim MST          -- O((V+E) log V)  Unit 3
============================================================
*/

#include "graph.h"

// Forward declaration needed for Dijkstra's negative-edge fallback
void bellmanFord(const Graph& g, int src);

// ============================================================
//  PATH HELPER -- reconstruct path from parent array
// ============================================================
vector<int> buildPath(const vector<int>& parent, int src, int dst) {
    vector<int> path;
    for (int c = dst; c != -1; c = parent[c]) path.push_back(c);
    if (path.empty() || path.back() != src) return vector<int>();
    reverse(path.begin(), path.end());
    return path;
}

void printPath(const Graph& g, const vector<int>& path) {
    for (int i = 0; i < (int)path.size(); ++i) {
        if (i) cout << " -> ";
        cout << g.name(path[i]);
    }
}

// ============================================================
//  1. BINARY SEARCH  (Unit 1)
//  Applied: O(log V) lookup in a sorted routing table
//  vs O(V) linear scan -- shows why sorting matters
// ============================================================
int binarySearchRoute(const vector<RouteEntry>& table, int dest) {
    int lo = 0, hi = (int)table.size() - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if      (table[mid].destination == dest) return mid;
        else if (table[mid].destination <  dest) lo = mid + 1;
        else                                     hi = mid - 1;
    }
    return -1;
}

void demoBinarySearch(const Graph& g) {
    printHeader("1. Binary Search -- Routing Table Lookup");
    printExplain(
        "BINARY SEARCH",
        "Searches a sorted list by repeatedly halving the search range.\n"
        "  Applied here to look up the next-hop router in a routing table.",
        "O(log V) per lookup  vs  O(V) for linear scan"
    );

    vector<RouteEntry> table;
    for (int i = 0; i < g.V; ++i)
        table.push_back({i, max(0, i-1), i * 4});
    sort(table.begin(), table.end());

    cout << "  " << left << setw(16) << "Destination"
         << setw(12) << "Next Hop" << "Cost\n";
    printSep();
    for (auto& r : table)
        cout << "  " << setw(16) << g.name(r.destination)
             << setw(12) << g.name(r.nextHop)
             << r.cost << "ms\n";

    int query = 7;
    int idx = binarySearchRoute(table, query);
    cout << "\n  Query: find entry for " << g.name(query) << "\n";
    if (idx != -1)
        cout << "  Found at index [" << idx << "] "
             << "-> next hop = " << g.name(table[idx].nextHop)
             << ", cost = " << table[idx].cost << "ms\n";
    cout << "\n  >> Sorted table enables O(log V) lookup.\n"
         << "     Without sorting, we'd scan all V entries = O(V).\n";
}

// ============================================================
//  2. BFS  (Unit 3)
//  Applied: find path with fewest hops (router stops)
// ============================================================
void bfs(const Graph& g, int src) {
    printHeader("2. BFS -- Fewest-Hop Paths");
    printExplain(
        "BFS (Breadth-First Search)",
        "Explores neighbours layer by layer. Guarantees the path with\n"
        "  fewest hops (edges), but ignores edge weights (latency).",
        "O(V + E)"
    );
    cout << "  Source: " << g.name(src) << "\n\n";

    vector<int> dist(g.V, -1), parent(g.V, -1);
    queue<int> q;
    dist[src] = 0; q.push(src);
    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (auto edge : g.adj[u]) { // C++11 Fix
            int v = edge.first;
            if (dist[v] == -1) { dist[v] = dist[u]+1; parent[v] = u; q.push(v); }
        }
    }

    cout << "  " << left << setw(8) << "Dest"
         << setw(8) << "Hops" << "Path\n";
    printSep();
    for (int v = 0; v < g.V; ++v) {
        cout << "  " << setw(8) << g.name(v);
        if (dist[v] == -1) { cout << setw(8) << "INF" << "unreachable\n"; continue; }
        cout << setw(8) << dist[v];
        printPath(g, buildPath(parent, src, v));
        cout << "\n";
    }
    cout << "\n  >> BFS is ideal when all links have equal cost.\n"
         << "     Use Dijkstra when links have different latencies.\n";
}

// ============================================================
//  3. DFS  (Unit 3)
//  Applied: explore all reachable routers, detect unreachable nodes
// ============================================================
void dfs(const Graph& g, int src) {
    printHeader("3. DFS -- Network Reachability");
    printExplain(
        "DFS (Depth-First Search)",
        "Dives as deep as possible along each path before backtracking.\n"
        "  Used to find all routers reachable from a source.",
        "O(V + E)"
    );
    cout << "  Source: " << g.name(src) << "\n\n";

    vector<bool> visited(g.V, false);
    function<void(int, int)> go = [&](int u, int depth) {
        visited[u] = true;
        cout << string(depth * 2, ' ') << "-> " << g.name(u) << "\n";
        for (auto edge : g.adj[u]) { // C++11 Fix
            int v = edge.first;
            if (!visited[v]) go(v, depth + 1);
        }
    };
    go(src, 1);

    cout << "\n  Unreachable from " << g.name(src) << ": ";
    bool any = false;
    for (int i = 0; i < g.V; ++i)
        if (!visited[i]) { cout << g.name(i) << " "; any = true; }
    if (!any) cout << "none -- all routers reachable";
    cout << "\n\n  >> Routers that are unreachable will never receive traffic.\n"
         << "     DFS identifies isolated segments of the network.\n";
}

// ============================================================
//  4. CYCLE DETECTION  (Unit 3)
//  Applied: detect routing loops before forwarding packets
//  Method: DFS 3-colouring (WHITE=0, GREY=1, BLACK=2)
// ============================================================
bool cycleUtil(const Graph& g, int u, vector<int>& color, vector<int>& cyc) {
    color[u] = 1;
    for (auto edge : g.adj[u]) { // C++11 Fix
        int v = edge.first;
        if (color[v] == 1) { cyc.push_back(v); cyc.push_back(u); return true; }
        if (color[v] == 0 && cycleUtil(g, v, color, cyc)) {
            cyc.push_back(u); return true;
        }
    }
    color[u] = 2;
    return false;
}

void detectCycle(const Graph& g) {
    printHeader("4. Cycle Detection -- Routing Loop Prevention");
    printExplain(
        "CYCLE DETECTION (DFS 3-colour)",
        "Marks each node WHITE (unvisited), GREY (in current path),\n"
        "  or BLACK (done). A back-edge to a GREY node means a cycle\n"
        "  (routing loop) exists.",
        "O(V + E)"
    );

    vector<int> color(g.V, 0), cyc;
    bool found = false;
    for (int i = 0; i < g.V && !found; ++i)
        if (color[i] == 0) found = cycleUtil(g, i, color, cyc);

    if (found) {
        cout << "  [!] ROUTING LOOP DETECTED\n  Cycle involves: ";
        reverse(cyc.begin(), cyc.end());
        for (int i = 0; i < (int)cyc.size(); ++i) {
            if (i) cout << " -> ";
            cout << g.name(cyc[i]);
        }
        cout << "\n";
    } else {
        cout << "  [OK] No routing loops found. Network is loop-free.\n";
    }
    cout << "\n  >> Routing loops cause packets to circulate forever.\n"
         << "     Detected and blocked before any packet is forwarded.\n";
}

// ============================================================
//  5. TOPOLOGICAL SORT  (Unit 3)
//  Applied: determine safe packet-forwarding order
//  Method: Kahn's algorithm (BFS on in-degrees)
// ============================================================
void topologicalSort(const Graph& g) {
    printHeader("5. Topological Sort -- Safe Forwarding Order");
    printExplain(
        "TOPOLOGICAL SORT (Kahn's Algorithm)",
        "Orders routers so that every link goes from earlier to later\n"
        "  in the order. Packets can then be forwarded without waiting\n"
        "  for a dependency that hasn't been resolved yet.",
        "O(V + E)"
    );

    vector<int> indeg(g.V, 0);
    for (int u = 0; u < g.V; ++u)
        for (auto edge : g.adj[u]) indeg[edge.first]++; // C++11 Fix

    queue<int> q;
    for (int i = 0; i < g.V; ++i) if (indeg[i] == 0) q.push(i);

    vector<int> order;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        order.push_back(u);
        for (auto edge : g.adj[u]) { // C++11 Fix
            int v = edge.first;
            if (--indeg[v] == 0) q.push(v);
        }
    }

    if ((int)order.size() != g.V) {
        cout << "  [!] Graph has a cycle -- topological sort not possible.\n";
        return;
    }
    cout << "  Safe forwarding order:\n  ";
    for (int i = 0; i < (int)order.size(); ++i) {
        if (i) cout << " -> ";
        cout << g.name(order[i]);
    }
    cout << "\n\n  >> Forward packets in this order to avoid deadlocks.\n";
}

// ============================================================
//  6. DIJKSTRA  (Unit 3)  [GREEDY]
//  Applied: find lowest-latency path from source to all routers
// ============================================================
void dijkstra(const Graph& g, int src) {
    printHeader("6. Dijkstra -- Minimum Latency Paths (Greedy)");

    // NEGATIVE EDGE FALLBACK LOGIC
    if (g.hasNegativeEdge) {
        cout << "  [!] ERROR: Negative edge detected in network topology.\n"
             << "      Dijkstra's algorithm cannot handle negative weights.\n"
             << "      Automatically rerouting to Bellman-Ford...\n";
        printSep();
        bellmanFord(g, src); // Safe fallback
        return;
    }

    printExplain(
        "DIJKSTRA'S ALGORITHM",
        "Greedily picks the unvisited router with the smallest known\n"
        "  distance and relaxes its neighbours. Builds shortest paths\n"
        "  one by one. Cannot handle negative edge weights.",
        "O((V + E) log V)  --  uses a min-heap priority queue"
    );
    cout << "  Source: " << g.name(src) << "\n\n";

    vector<int> dist(g.V, INF), parent(g.V, -1);
    dist[src] = 0;
    
    // C++11 Fix: explicit comparator syntax for priority queue
    priority_queue<pair<int,int>, vector<pair<int,int> >, greater<pair<int,int> > > pq;
    pq.push({0, src});

    while (!pq.empty()) {
        int d = pq.top().first;  // C++11 Fix
        int u = pq.top().second; // C++11 Fix
        pq.pop();
        
        if (d > dist[u]) continue;
        for (auto edge : g.adj[u]) { // C++11 Fix
            int v = edge.first;
            int w = edge.second;
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                parent[v] = u;
                pq.push({dist[v], v});
            }
        }
    }

    cout << "  " << left << setw(8) << "Dest"
         << setw(14) << "Latency(ms)" << "Path\n";
    printSep();
    for (int v = 0; v < g.V; ++v) {
        cout << "  " << setw(8) << g.name(v);
        if (dist[v] == INF) { cout << setw(14) << "INF" << "unreachable\n"; continue; }
        cout << setw(14) << dist[v];
        printPath(g, buildPath(parent, src, v));
        cout << "\n";
    }
    cout << "\n  >> Dijkstra gives the optimal (lowest ms) route.\n"
         << "     Choose this for real-time packet forwarding decisions.\n";
}

// ============================================================
//  7. BELLMAN-FORD  (Unit 3)  [DP Relaxation]
//  Applied: paths with penalty costs, detect negative cycles
// ============================================================
void bellmanFord(const Graph& g, int src) {
    printHeader("7. Bellman-Ford -- Paths with Penalty Costs (DP)");
    printExplain(
        "BELLMAN-FORD ALGORITHM",
        "Relaxes ALL edges V-1 times. Slower than Dijkstra but handles\n"
        "  negative weights (penalty costs on congested links) and can\n"
        "  detect negative cycles (unstable routing loops).",
        "O(V * E)  --  DP-style edge relaxation"
    );
    cout << "  Source: " << g.name(src) << "\n\n";

    vector<int> dist(g.V, INF), parent(g.V, -1);
    dist[src] = 0;

    for (int iter = 0; iter < g.V - 1; ++iter) {
        bool updated = false;
        for (auto& e : g.edges) {
            if (dist[e.u] != INF && dist[e.u] + e.weight < dist[e.v]) {
                dist[e.v] = dist[e.u] + e.weight;
                parent[e.v] = e.u;
                updated = true;
            }
        }
        if (!updated) {
            cout << "  Early termination at iteration " << iter+1
                 << " (no changes)\n\n";
            break;
        }
    }

    for (auto& e : g.edges) {
        if (dist[e.u] != INF && dist[e.u] + e.weight < dist[e.v]) {
            cout << "  [!] Negative cycle detected -- routing is unstable!\n";
            return;
        }
    }

    cout << "  " << left << setw(8) << "Dest"
         << setw(14) << "Cost(ms)" << "Path\n";
    printSep();
    for (int v = 0; v < g.V; ++v) {
        cout << "  " << setw(8) << g.name(v);
        if (dist[v] == INF) { cout << setw(14) << "INF" << "unreachable\n"; continue; }
        cout << setw(14) << dist[v];
        printPath(g, buildPath(parent, src, v));
        cout << "\n";
    }
    cout << "\n  >> Use Bellman-Ford when links have penalty/negative costs.\n"
         << "     Result matches Dijkstra here since all weights are positive.\n";
}

// ============================================================
//  8. FLOYD-WARSHALL  (Unit 3)  [All-Pairs DP]
//  Applied: build complete routing table for all router pairs
// ============================================================
void floydWarshall(const Graph& g) {
    printHeader("8. Floyd-Warshall -- Complete Routing Table (All Pairs)");
    printExplain(
        "FLOYD-WARSHALL ALGORITHM",
        "Dynamic programming: for every pair (i,j) check if going\n"
        "  through an intermediate node k is shorter. Builds the full\n"
        "  routing table -- every router to every other router.",
        "O(V^3) time  |  O(V^2) space"
    );

    vector<vector<int> > dist(g.V, vector<int>(g.V, INF));
    vector<vector<int> > nxt(g.V,  vector<int>(g.V, -1));
    for (int i = 0; i < g.V; ++i) dist[i][i] = 0;
    
    for (int u = 0; u < g.V; ++u)
        for (auto edge : g.adj[u]) { // C++11 Fix
            int v = edge.first;
            int w = edge.second;
            if (w < dist[u][v]) { dist[u][v] = w; nxt[u][v] = v; }
        }

    for (int k = 0; k < g.V; ++k)
        for (int i = 0; i < g.V; ++i)
            for (int j = 0; j < g.V; ++j)
                if (dist[i][k] != INF && dist[k][j] != INF &&
                    dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    nxt[i][j]  = nxt[i][k];
                }

    // Print matrix (abbreviated)
    cout << "  Distance matrix (ms) -- (-1 = unreachable):\n\n";
    cout << setw(7) << " ";
    for (int j = 0; j < g.V; ++j)
        cout << setw(6) << g.name(j).substr(0,4);
    cout << "\n  " << string(6 + 6*g.V, '-') << "\n";
    for (int i = 0; i < g.V; ++i) {
        cout << "  " << setw(5) << g.name(i).substr(0,4) << "|";
        for (int j = 0; j < g.V; ++j)
            cout << setw(6) << (dist[i][j] == INF ? -1 : dist[i][j]);
        cout << "\n";
    }

    // Example path
    int s = 0, d = g.V-1;
    cout << "\n  Example -- best path " << g.name(s)
         << " to " << g.name(d) << ": ";
    if (dist[s][d] == INF) { cout << "unreachable\n"; }
    else {
        for (int c = s; c != d; c = nxt[c][d]) cout << g.name(c) << " -> ";
        cout << g.name(d) << "  [" << dist[s][d] << "ms]\n";
    }
    cout << "\n  >> Floyd-Warshall builds the full routing table at once.\n"
         << "     Any router can instantly look up the best path to any other.\n";
}

// ============================================================
//  9. KRUSKAL MST  (Unit 3)  [Greedy + DSU]
//  Applied: find minimum cost backbone to connect all routers
// ============================================================
void kruskalMST(const Graph& g) {
    printHeader("9. Kruskal MST -- Minimum Cost Network Backbone");
    printExplain(
        "KRUSKAL'S ALGORITHM",
        "Sorts all edges by weight, then greedily adds the cheapest\n"
        "  edge that doesn't create a cycle (checked via DSU).\n"
        "  Result: cheapest set of links connecting all routers.",
        "O(E log E)  --  sort + near-O(1) DSU operations"
    );

    vector<Edge> ue = g.getUndirectedEdges();
    sort(ue.begin(), ue.end());
    DSU dsu(g.V);
    vector<Edge> mst;
    int total = 0;

    for (auto& e : ue)
        if (dsu.unite(e.u, e.v)) { mst.push_back(e); total += e.weight; }

    cout << "  " << left << setw(10) << "From"
         << setw(10) << "To"
         << setw(14) << "Weight(ms)"
         << "Running Total\n";
    printSep();
    int running = 0;
    for (auto& e : mst) {
        running += e.weight;
        cout << "  " << setw(10) << g.name(e.u)
             << setw(10) << g.name(e.v)
             << setw(14) << e.weight
             << running << "ms\n";
    }
    cout << "\n  Total backbone cost: " << total << "ms\n";
    cout << "\n  >> MST gives the cheapest wiring to connect all routers.\n"
         << "     Used for physical network planning and backbone design.\n";
}

// ============================================================
//  10. PRIM MST  (Unit 3)  [Greedy]
//  Applied: grow backbone incrementally from a start router
// ============================================================
void primMST(const Graph& g, int start = 0) {
    printHeader("10. Prim MST -- Incremental Backbone Growth");
    printExplain(
        "PRIM'S ALGORITHM",
        "Starts from one router and greedily grows the MST by always\n"
        "  picking the cheapest edge that connects a new router.\n"
        "  Equivalent result to Kruskal, different approach.",
        "O((V + E) log V)  --  min-heap priority queue"
    );
    cout << "  Starting router: " << g.name(start) << "\n\n";

    vector<int> key(g.V, INF), parent(g.V, -1);
    vector<bool> inMST(g.V, false);
    key[start] = 0;
    
    // C++11 Fix: explicit comparator syntax
    priority_queue<pair<int,int>, vector<pair<int,int> >, greater<pair<int,int> > > pq;
    pq.push({0, start});

    cout << "  " << left << setw(10) << "Added"
         << setw(16) << "Via" << "Cost\n";
    printSep();
    int total = 0;
    while (!pq.empty()) {
        int k = pq.top().first;  // C++11 Fix
        int u = pq.top().second; // C++11 Fix
        pq.pop();
        
        if (inMST[u]) continue;
        inMST[u] = true; total += k;
        
        if (parent[u] == -1)
            cout << "  " << setw(10) << g.name(u) << setw(16) << "(start)" << 0 << "\n";
        else
            cout << "  " << setw(10) << g.name(u)
                 << setw(16) << (g.name(parent[u]) + "->" + g.name(u))
                 << k << "ms\n";
                 
        for (auto edge : g.adj[u]) { // C++11 Fix
            int v = edge.first;
            int w = edge.second;
            if (!inMST[v] && w < key[v]) { key[v] = w; parent[v] = u; pq.push({w,v}); }
        }
    }
    cout << "\n  Total backbone cost: " << total << "ms\n";
    cout << "\n  >> Prim and Kruskal produce equivalent MSTs.\n"
         << "     Prim works better on dense graphs, Kruskal on sparse ones.\n";
}

// ============================================================
//  COMPLEXITY COMPARISON TABLE
// ============================================================
void printComplexityTable() {
    printHeader("Algorithm Complexity Summary");
    cout << "\n  " << left
         << setw(20) << "Algorithm"
         << setw(22) << "Time"
         << setw(16) << "Space"
         << "Notes\n";
    printSep();
    auto row = [](const string& n, const string& t,
                  const string& s, const string& note) {
        cout << "  " << left << setw(20) << n << setw(22) << t
             << setw(16) << s << note << "\n";
    };
    row("Binary Search",   "O(log V)",       "O(1)",    "Unit 1 - routing table lookup");
    row("BFS",             "O(V + E)",        "O(V)",    "Fewest hops, ignores weights");
    row("DFS",             "O(V + E)",        "O(V)",    "Reachability, used by SCC");
    row("Cycle Detection", "O(V + E)",        "O(V)",    "3-colour DFS");
    row("Topo Sort",       "O(V + E)",        "O(V)",    "Kahn's BFS on in-degrees");
    row("Dijkstra",        "O((V+E) log V)",  "O(V)",    "Greedy, no negative weights");
    row("Bellman-Ford",    "O(V * E)",        "O(V)",    "DP, handles negative weights");
    row("Floyd-Warshall",  "O(V^3)",          "O(V^2)",  "All-pairs DP");
    row("Kruskal MST",     "O(E log E)",      "O(E)",    "Greedy + DSU");
    row("Prim MST",        "O((V+E) log V)",  "O(V)",    "Greedy + min-heap");
    cout << "\n  When to choose which shortest-path algorithm:\n";
    cout << "    Dijkstra       -> all weights >= 0, fastest in practice\n";
    cout << "    Bellman-Ford   -> negative/penalty weights, cycle detection\n";
    cout << "    Floyd-Warshall -> need ALL pairs, V is small (V <= 500)\n";
}

// ============================================================
//  MAIN
// ============================================================
int main() {
    printHeader("NETWORK TRAFFIC OPTIMIZER -- ROUTING ENGINE");
    cout << "  Member 1  |  Units 1 + 3\n";

    Graph g = buildNetwork();
    g.printAdjList();
    g.printSummary();

    demoBinarySearch(g);
    bfs(g, 0);
    dfs(g, 0);
    detectCycle(g);
    topologicalSort(g);
    dijkstra(g, 0);
    bellmanFord(g, 0);
    floydWarshall(g);
    kruskalMST(g);
    primMST(g, 0);
    printComplexityTable();

    cout << "\n[DONE] All routing algorithms completed.\n\n";
    return 0;
}
