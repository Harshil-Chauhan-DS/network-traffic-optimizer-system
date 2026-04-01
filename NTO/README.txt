============================================================
  NETWORK TRAFFIC OPTIMIZER
  Design & Analysis of Algorithms -- Undergraduate Project
============================================================

TEAM
----
  Member 1 -- Routing Engine       (routing_engine.cpp)
  Member 2 -- Packet Scheduler     (packet_scheduler.cpp)
  Member 3 -- Traffic Optimizer    (traffic_optimizer.cpp)
  Member 4 -- Lookup & Inspection  (lookup_inspect.cpp)

SYLLABUS COVERAGE
-----------------
  Unit 1 : Asymptotic Notations, Binary Search, Recursion
  Unit 2 : All sorting algorithms (Heap, Merge, Quick,
           Randomized Quick, Counting, Insertion, Selection, Bubble)
  Unit 3 : Graph algorithms (BFS, DFS, Topological Sort,
           Cycle Detection, SCC, Dijkstra, Bellman-Ford,
           Floyd-Warshall, Kruskal MST, Prim MST)
  Unit 4 : Greedy (Activity Selection, Job Sequencing,
           Fractional Knapsack, Huffman Codes)
           DP (0/1 Knapsack, Fibonacci Memoization vs Tabulation)
  Unit 5 : Hashing (Chaining, Open Addressing), String Matching
           (Naive, KMP, Rabin-Karp), NP-Completeness (TSP)

PROJECT STRUCTURE
-----------------
  graph.h               -- Shared data layer (ALL members include this)
  main.cpp              -- Unified program with menu (SUBMIT THIS)
  routing_engine.cpp    -- Member 1 module
  packet_scheduler.cpp  -- Member 2 module
  traffic_optimizer.cpp -- Member 3 module
  lookup_inspect.cpp    -- Member 4 module
  network.txt           -- Sample custom network input file
  Makefile              -- Build script
  README.txt            -- This file

HOW TO COMPILE
--------------
  Option A -- Build everything at once (recommended):
    make

  Option B -- Build unified program only:
    g++ -std=c++17 -O2 -o nto main.cpp

  Option C -- Build individual modules:
    g++ -std=c++17 -O2 -o routing_engine    routing_engine.cpp
    g++ -std=c++17 -O2 -o packet_scheduler  packet_scheduler.cpp
    g++ -std=c++17 -O2 -o traffic_optimizer traffic_optimizer.cpp
    g++ -std=c++17 -O2 -o lookup_inspect    lookup_inspect.cpp

  Clean compiled binaries:
    make clean

HOW TO RUN
----------
  Unified program (recommended for demo):
    ./nto

  On startup you will see:
    1  Use built-in 12-node demo network
    2  Load from file  (e.g. network.txt)
    3  Enter manually via terminal

  Then the main menu lets you run any single module or all at once.

CUSTOM NETWORK FILE FORMAT (network.txt)
-----------------------------------------
  Line 1 : V E          (number of nodes, number of directed edges)
  Line 2 : name0 name1 ... nameV-1   (node names, space-separated)
  Lines  : u v w        (directed edge from u to v, weight w in ms)

  Node indices start at 0.
  Example:
    4 4
    A B C D
    0 1 10
    1 2 5
    2 3 7
    0 3 20

DEMO NETWORK
------------
  The built-in demo network has 12 routers (R0-R11) and 19 directed
  edges representing an ISP-like topology with latencies in ms.

  The sample network.txt has 8 Indian cities as routers.

ALGORITHM EXPLANATIONS
----------------------
  Each algorithm prints a short explanation before running:
    - What it does
    - How it applies to network traffic
    - Time and space complexity

  The complexity comparison table is printed at the end of Module 1.
  The sorting comparison table is printed at the end of Module 2.

REQUIREMENTS
------------
  - C++ compiler with C++17 support (g++ 7+ or clang++ 5+)
  - Standard library only -- no external dependencies
  - Works on Windows (MinGW/MSVC), Linux, macOS
  - All output is ASCII-safe (no Unicode box characters)

============================================================
