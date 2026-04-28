/*
============================================================
  main.cpp  --  GUI Receiver Endpoint (C++11)
  Network Traffic Optimizer
============================================================
*/

#define main main_routing
#include "routing_engine.cpp"
#undef  main

#define main main_scheduler
#include "packet_scheduler.cpp"
#undef  main

#define main main_optimizer
#include "traffic_optimizer.cpp"
#undef  main

#define main main_lookup
#include "lookup_inspect.cpp"
#undef  main

int main() {
    // 1. Read Network Choice
    int netChoice;
    if (!(cin >> netChoice)) return 0;

    Graph g(0, vector<string>());

    if (netChoice == 1) {
        g = buildTestNetwork();
    } 
    else if (netChoice == 2) {
        string filename;
        cin >> filename;
        g = loadFromFile(filename);
        if (g.V == 0) g = buildTestNetwork(); // Safe fallback
    } 
    else if (netChoice == 3) {
        int V, E;
        cin >> V;
        vector<string> names(V);
        for (int i = 0; i < V; ++i) cin >> names[i];
        g = Graph(V, names);
        cin >> E;
        for (int i = 0; i < E; ++i) {
            int u, v, w;
            cin >> u >> v >> w;
            if (u >= 0 && u < V && v >= 0 && v < V) g.addDirectedEdge(u, v, w);
        }
    }

    // 2. Read Module & Algorithm Choice
    int modChoice, algChoice;
    if (!(cin >> modChoice >> algChoice)) return 0;

    // 3. Execute exactly what the GUI requested
    if (modChoice == 1) { // Routing Engine
        if (algChoice == 1) dijkstra(g, 0);
        else if (algChoice == 2) bellmanFord(g, 0);
        else if (algChoice == 3) bfs(g, 0);
        else if (algChoice == 4) { dfs(g, 0); detectCycle(g); }
        else if (algChoice == 5) topologicalSort(g);
        else if (algChoice == 6) floydWarshall(g);
        else if (algChoice == 7) { kruskalMST(g); cout << "\n"; primMST(g, 0); }
    } 
    else if (modChoice == 2) { // Packet Scheduler
        vector<Packet> pkts = buildTestPackets();
        if (algChoice == 1) demoHeapSort(pkts);
        else if (algChoice == 2) demoMergeSort(pkts);
        else if (algChoice == 3) demoQuickSort(pkts);
        else if (algChoice == 4) demoCountingSort(pkts);
        else if (algChoice == 5) schedulingSimulation(pkts);
    }
    else if (modChoice == 3) { // Traffic Optimizer
        if (algChoice == 1) activitySelection();
        else if (algChoice == 2) jobSequencing();
        else if (algChoice == 3) fractionalKnapsack();
        else if (algChoice == 4) knapsack01();
        else if (algChoice == 5) huffmanCodes();
    }
    else if (modChoice == 4) { // Lookup & Inspection
        if (algChoice == 1) demoChaining();
        else if (algChoice == 2) demoOpenAddr();
        else if (algChoice == 3) kmpMatch();
        else if (algChoice == 4) rabinKarp();
        else if (algChoice == 5) npDiscussion(g);
    }

    return 0;
}
