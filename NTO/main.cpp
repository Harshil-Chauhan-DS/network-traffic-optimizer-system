/*
============================================================
  main.cpp  --  Unified Entry Point
  Network Traffic Optimizer
============================================================
  Compile everything with:
    make
  Or manually:
    g++ -std=c++17 -O2 -o nto main.cpp
  Run:
    ./nto

  This file includes all four modules and presents a single
  menu so the professor can run any or all of them in one
  session on the same network.

  Modules:
    Module 1 -- Routing Engine       (Member 1, Unit 3)
    Module 2 -- Packet Scheduler     (Member 2, Unit 2)
    Module 3 -- Traffic Optimizer    (Member 3, Unit 4)
    Module 4 -- Lookup & Inspection  (Member 4, Unit 5)
============================================================
*/

// ============================================================
//  Guard against duplicate includes -- each .cpp normally
//  has its own main(). We rename them here before including.
// ============================================================
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

// ============================================================
//  UNIFIED MAIN
// ============================================================
int main() {
    printHeader("NETWORK TRAFFIC OPTIMIZER");
    cout << "  A Design & Analysis of Algorithms Project\n";
    cout << "  Team of 4  |  Units 1-5\n";

    // Build the shared network once -- all modules use it
    Graph g = buildNetwork();
    g.printAdjList();
    g.printSummary();

    while (true) {
        printHeader("Main Menu");
        cout << "\n  1  Routing Engine       (BFS, DFS, Dijkstra, Bellman-Ford,\n"
             << "                           Floyd-Warshall, MST, Cycle, SCC)\n"
             << "  2  Packet Scheduler     (Heap, Merge, Quick, Counting,\n"
             << "                           Insertion, Selection, Bubble sort)\n"
             << "  3  Traffic Optimizer    (Activity Selection, Job Sequencing,\n"
             << "                           Knapsack, Huffman, Fibonacci DP)\n"
             << "  4  Lookup & Inspection  (Hash Tables, KMP, Rabin-Karp,\n"
             << "                           Naive Matching, NP-Completeness)\n"
             << "  5  Run ALL modules\n"
             << "  0  Exit\n\n"
             << "  Choice: ";

        int choice;
        if (!(cin >> choice)) { cin.clear(); cin.ignore(1000,'\n'); continue; }

        switch (choice) {

        case 1:
            printHeader("MODULE 1 -- ROUTING ENGINE");
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
            break;

        case 2: {
            printHeader("MODULE 2 -- PACKET SCHEDULER");
            auto pkts = buildTestPackets();
            printPackets(pkts, "Input: 20 unsorted packets");
            demoHeapSort(pkts);
            demoMergeSort(pkts);
            demoQuickSort(pkts);
            demoCountingSort(pkts);
            demoInsertionSort(pkts);
            demoSelectionSort(pkts);
            demoBubbleSort(pkts);
            printSortComparison();
            schedulingSimulation(pkts);
            break;
        }

        case 3:
            printHeader("MODULE 3 -- TRAFFIC OPTIMIZER");
            activitySelection();
            jobSequencing();
            fractionalKnapsack();
            knapsack01();
            fibonacciComparison();
            huffmanCodes();
            break;

        case 4:
            printHeader("MODULE 4 -- LOOKUP & INSPECTION");
            demoChaining();
            demoOpenAddr();
            naiveMatch();
            kmpMatch();
            rabinKarp();
            npDiscussion(g);
            break;

        case 5:
            printHeader("RUNNING ALL MODULES");

            printHeader("MODULE 1 -- ROUTING ENGINE");
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

            {
            printHeader("MODULE 2 -- PACKET SCHEDULER");
            auto pkts = buildTestPackets();
            printPackets(pkts, "Input: 20 unsorted packets");
            demoHeapSort(pkts);
            demoMergeSort(pkts);
            demoQuickSort(pkts);
            demoCountingSort(pkts);
            demoInsertionSort(pkts);
            demoSelectionSort(pkts);
            demoBubbleSort(pkts);
            printSortComparison();
            schedulingSimulation(pkts);
            }

            printHeader("MODULE 3 -- TRAFFIC OPTIMIZER");
            activitySelection();
            jobSequencing();
            fractionalKnapsack();
            knapsack01();
            fibonacciComparison();
            huffmanCodes();

            printHeader("MODULE 4 -- LOOKUP & INSPECTION");
            demoChaining();
            demoOpenAddr();
            naiveMatch();
            kmpMatch();
            rabinKarp();
            npDiscussion(g);

            cout << "\n[DONE] All modules completed.\n";
            break;

        case 0:
            cout << "\n  Goodbye!\n\n";
            return 0;

        default:
            cout << "  Invalid choice. Enter 0-5.\n";
        }

        cout << "\n  Press Enter to return to menu...";
        cin.ignore(1000, '\n');
        cin.get();
    }
    return 0;
}
