/*
============================================================
  lookup_inspect.cpp  --  Member 4  |  Unit 5
  Network Traffic Optimizer
============================================================
  Compile:
    g++ -std=c++17 -O2 -o lookup_inspect lookup_inspect.cpp
  Run:
    ./lookup_inspect

  Algorithms:
    1. Hash Table (Chaining)      -- O(1) avg  IP lookup
    2. Hash Table (Open Addr.)    -- O(1) avg  comparison
    3. Naive String Matching      -- O(n*m)    baseline
    4. KMP Algorithm              -- O(n+m)    fast inspection
    5. Rabin-Karp Algorithm       -- O(n+m)    hash-based
    6. NP-Completeness Discussion -- TSP / why optimal routing is hard
============================================================
*/

#include "graph.h"

// ============================================================
//  1. HASH TABLE WITH CHAINING  (Unit 5)
//  Applied: O(1) average routing table lookup by IP address
// ============================================================
class HashTableChaining {
    int size;
    vector<list<pair<string,string>>> table;

    int hash(const string& key) const {
        int h = 0;
        for (char c : key) h = (h * 31 + c) % size;
        return (h + size) % size;
    }

public:
    HashTableChaining(int sz) : size(sz), table(sz) {}

    void insert(const string& ip, const string& nextHop) {
        int h = hash(ip);
        for (auto& [k,v] : table[h])
            if (k == ip) { v = nextHop; return; }
        table[h].push_back({ip, nextHop});
    }

    string lookup(const string& ip) const {
        int h = hash(ip);
        for (auto& [k,v] : table[h])
            if (k == ip) return v;
        return "NOT FOUND";
    }

    void printTable() const {
        cout << "  " << left << setw(6) << "Slot" << "Entries\n";
        printSep();
        for (int i = 0; i < size; ++i) {
            if (table[i].empty()) continue;
            cout << "  " << setw(6) << i;
            for (auto& [k,v] : table[i])
                cout << "[" << k << " -> " << v << "]  ";
            cout << "\n";
        }
    }

    int maxChainLength() const {
        int mx = 0;
        for (auto& chain : table) mx = max(mx, (int)chain.size());
        return mx;
    }
};

void demoChaining() {
    printHeader("1. Hash Table (Chaining) -- IP Address Routing");
    printExplain(
        "HASH TABLE WITH CHAINING",
        "Maps IP addresses to next-hop routers using a hash function.\n"
        "  Collisions are resolved by maintaining a linked list (chain)\n"
        "  at each bucket. Average O(1) lookup even with collisions.",
        "O(1) avg insert/lookup  |  O(n) worst  |  O(n + m) space"
    );

    HashTableChaining ht(11); // prime table size reduces collisions

    vector<pair<string,string>> routes = {
        {"192.168.1.1","R1"}, {"10.0.0.1","R3"}, {"172.16.0.1","R5"},
        {"192.168.1.2","R2"}, {"10.0.0.2","R4"}, {"172.16.0.2","R6"},
        {"192.168.2.1","R7"}, {"10.0.1.1","R8"}, {"172.17.0.1","R9"},
    };

    for (auto& [ip, hop] : routes) ht.insert(ip, hop);

    ht.printTable();
    cout << "  Max chain length: " << ht.maxChainLength() << "\n";

    cout << "\n  Lookup tests:\n";
    for (auto& ip : {"192.168.1.1", "10.0.0.2", "99.99.99.99"}) {
        cout << "  " << left << setw(18) << ip
             << "-> " << ht.lookup(ip) << "\n";
    }
    cout << "\n  >> Chaining handles collisions gracefully.\n"
         << "     Table size should be prime to spread entries evenly.\n";
}

// ============================================================
//  2. HASH TABLE WITH OPEN ADDRESSING  (Unit 5)
//  Applied: alternative collision handling, cache-friendly
// ============================================================
class HashTableOpenAddr {
    int size;
    vector<pair<string,string>> table; // {"",""}=empty, {"#",""}=deleted
    int probes = 0;

    int hash(const string& key) const {
        int h = 0;
        for (char c : key) h = (h * 31 + c) % size;
        return (h + size) % size;
    }

public:
    HashTableOpenAddr(int sz) : size(sz), table(sz, {"",""}) {}

    void insert(const string& ip, const string& hop) {
        int h = hash(ip), i = 0;
        while (table[(h+i)%size].first != "" &&
               table[(h+i)%size].first != "#" &&
               table[(h+i)%size].first != ip)
            i++;
        table[(h+i)%size] = {ip, hop};
    }

    string lookup(const string& ip) {
        int h = hash(ip), i = 0;
        while (table[(h+i)%size].first != "") {
            probes++;
            if (table[(h+i)%size].first == ip)
                return table[(h+i)%size].second;
            i++;
            if (i >= size) break;
        }
        return "NOT FOUND";
    }

    void printTable() const {
        cout << "  " << left << setw(6) << "Slot"
             << setw(20) << "IP" << "Next Hop\n";
        printSep();
        for (int i = 0; i < size; ++i) {
            if (table[i].first == "" || table[i].first == "#") continue;
            cout << "  " << setw(6) << i
                 << setw(20) << table[i].first
                 << table[i].second << "\n";
        }
    }

    int getProbes() const { return probes; }
};

void demoOpenAddr() {
    printHeader("2. Hash Table (Open Addressing) -- Linear Probing");
    printExplain(
        "OPEN ADDRESSING (Linear Probing)",
        "When a collision occurs, scan forward to the next empty slot.\n"
        "  All entries stored directly in the array -- no linked lists.\n"
        "  More cache-friendly than chaining, but clusters can form.",
        "O(1) avg  |  O(n) worst  |  O(m) space  --  no extra pointers"
    );

    HashTableOpenAddr ht(13);
    vector<pair<string,string>> routes = {
        {"192.168.1.1","R1"}, {"10.0.0.1","R3"}, {"172.16.0.1","R5"},
        {"192.168.1.2","R2"}, {"10.0.0.2","R4"}, {"172.16.0.2","R6"},
    };
    for (auto& [ip, hop] : routes) ht.insert(ip, hop);
    ht.printTable();

    cout << "\n  Lookup tests:\n";
    for (auto& ip : {"192.168.1.1","172.16.0.2","55.55.55.55"})
        cout << "  " << left << setw(18) << ip
             << "-> " << ht.lookup(ip) << "\n";
    cout << "  Total probes used: " << ht.getProbes() << "\n";
    cout << "\n  Chaining vs Open Addressing:\n";
    cout << "    Chaining      -- handles high load better, needs extra memory\n";
    cout << "    Open Addr.    -- cache-friendly, degrades as table fills up\n";
}

// ============================================================
//  STRING MATCHING HELPERS
// ============================================================
void printMatchResult(const string& text, const string& pattern,
                      const vector<int>& matches, const string& algo,
                      int comparisons) {
    if (matches.empty()) {
        cout << "  Pattern not found.\n";
    } else {
        cout << "  Found at positions: ";
        for (int i = 0; i < (int)matches.size(); ++i) {
            if (i) cout << ", ";
            cout << matches[i];
        }
        cout << "\n";
    }
    cout << "  Comparisons made: " << comparisons << "\n";
}

// ============================================================
//  3. NAIVE STRING MATCHING  (Unit 5)
//  Applied: baseline packet payload inspection
// ============================================================
void naiveMatch() {
    printHeader("3. Naive String Matching -- Baseline Inspection");
    printExplain(
        "NAIVE STRING MATCHING",
        "Slide the pattern over the text one position at a time.\n"
        "  At each position, compare every character.\n"
        "  Simple but slow -- used as a baseline to show why\n"
        "  KMP and Rabin-Karp are needed.",
        "O(n * m)  --  n = text length, m = pattern length"
    );

    string text    = "TCPTCPUDPTCPICMPTCPBGP";
    string pattern = "TCP";
    cout << "  Text   : " << text << "\n";
    cout << "  Pattern: " << pattern << "\n\n";

    vector<int> matches;
    int comps = 0;
    int n = text.size(), m = pattern.size();
    for (int i = 0; i <= n-m; ++i) {
        int j = 0;
        while (j < m) { comps++; if (text[i+j] != pattern[j]) break; j++; }
        if (j == m) matches.push_back(i);
    }
    printMatchResult(text, pattern, matches, "Naive", comps);
    cout << "\n  >> Naive approach re-checks characters already seen.\n"
         << "     KMP avoids this with a failure function.\n";
}

// ============================================================
//  4. KMP ALGORITHM  (Unit 5)
//  Applied: fast deep packet inspection for malicious patterns
// ============================================================
vector<int> buildLPS(const string& pattern) {
    int m = pattern.size();
    vector<int> lps(m, 0);
    int len = 0, i = 1;
    while (i < m) {
        if (pattern[i] == pattern[len]) { lps[i++] = ++len; }
        else if (len) len = lps[len-1];
        else lps[i++] = 0;
    }
    return lps;
}

void kmpMatch() {
    printHeader("4. KMP Algorithm -- Fast Packet Inspection");
    printExplain(
        "KMP (Knuth-Morris-Pratt)",
        "Pre-processes the pattern to build a failure function (LPS array).\n"
        "  When a mismatch occurs, uses LPS to skip re-checking characters\n"
        "  already known to match. Applied to inspect packet payloads for\n"
        "  malicious strings (e.g. SQL injection, virus signatures).",
        "O(n + m)  --  O(m) preprocessing + O(n) matching"
    );

    string text    = "TCPTCPUDPTCPICMPTCPBGP";
    string pattern = "TCP";
    cout << "  Text   : " << text << "\n";
    cout << "  Pattern: " << pattern << "\n";

    vector<int> lps = buildLPS(pattern);
    cout << "  LPS    : ";
    for (int v : lps) cout << v << " ";
    cout << "\n  (LPS = Longest Proper Prefix which is also Suffix)\n\n";

    vector<int> matches;
    int comps = 0, n = text.size(), m = pattern.size();
    int i = 0, j = 0;
    while (i < n) {
        comps++;
        if (text[i] == pattern[j]) { i++; j++; }
        if (j == m) { matches.push_back(i-j); j = lps[j-1]; }
        else if (i < n && text[i] != pattern[j]) {
            if (j) j = lps[j-1]; else i++;
        }
    }
    printMatchResult(text, pattern, matches, "KMP", comps);
    cout << "\n  >> KMP used " << comps << " comparisons vs naive approach.\n"
         << "     Critical for real-time deep packet inspection (DPI).\n";
}

// ============================================================
//  5. RABIN-KARP  (Unit 5)
//  Applied: find multiple suspicious patterns simultaneously
// ============================================================
void rabinKarp() {
    printHeader("5. Rabin-Karp -- Hash-Based Pattern Matching");
    printExplain(
        "RABIN-KARP ALGORITHM",
        "Computes a rolling hash of the text window and compares it to\n"
        "  the pattern hash. Only does full character comparison when\n"
        "  hashes match (spurious hits). Best for multiple patterns\n"
        "  or when looking for any of many virus signatures.",
        "O(n + m) avg  |  O(n*m) worst (many hash collisions)"
    );

    string text    = "TCPTCPUDPTCPICMPTCPBGP";
    string pattern = "TCP";
    int d = 256, q = 101; // base and prime mod
    int n = text.size(), m = pattern.size();
    int h = 1, patHash = 0, txtHash = 0;
    int comps = 0, hashComps = 0;

    for (int i = 0; i < m-1; ++i) h = (h*d) % q;
    for (int i = 0; i < m; ++i) {
        patHash = (d*patHash + pattern[i]) % q;
        txtHash = (d*txtHash + text[i]) % q;
    }

    vector<int> matches;
    for (int i = 0; i <= n-m; ++i) {
        hashComps++;
        if (patHash == txtHash) {
            bool match = true;
            for (int j = 0; j < m; ++j) {
                comps++;
                if (text[i+j] != pattern[j]) { match = false; break; }
            }
            if (match) matches.push_back(i);
        }
        if (i < n-m)
            txtHash = (d*(txtHash - text[i]*h) + text[i+m]) % q;
        if (txtHash < 0) txtHash += q;
    }

    cout << "  Text   : " << text << "\n";
    cout << "  Pattern: " << pattern << "\n\n";
    printMatchResult(text, pattern, matches, "Rabin-Karp", comps);
    cout << "  Hash comparisons: " << hashComps
         << "  |  Char comparisons: " << comps << "\n";
    cout << "\n  Comparison of matching algorithms:\n";
    cout << "    Naive      -- O(n*m), simple, no preprocessing\n";
    cout << "    KMP        -- O(n+m), best for single pattern, guaranteed\n";
    cout << "    Rabin-Karp -- O(n+m) avg, best for multiple patterns\n";
}

// ============================================================
//  6. NP-COMPLETENESS  (Unit 5)
//  Applied: explain why finding the globally optimal route is NP-hard
// ============================================================
void npDiscussion(const Graph& g) {
    printHeader("6. NP-Completeness -- Why Optimal Routing is Hard");

    cout << "\n  WHAT IS NP-COMPLETENESS?\n";
    printSep();
    cout << "  P       : Problems solvable in polynomial time.\n";
    cout << "            Example: Dijkstra, BFS, sorting.\n\n";
    cout << "  NP      : Problems where a given solution can be VERIFIED\n";
    cout << "            in polynomial time, but not necessarily FOUND fast.\n\n";
    cout << "  NP-Hard : At least as hard as the hardest NP problems.\n";
    cout << "            No known polynomial-time algorithm exists.\n\n";
    cout << "  NP-Complete : Both NP and NP-Hard.\n\n";

    cout << "  THE TRAVELLING SALESMAN PROBLEM (TSP)\n";
    printSep();
    cout << "  Goal: visit every router EXACTLY ONCE and return to start,\n";
    cout << "  minimising total latency.\n\n";
    cout << "  Why it matters for routing:\n";
    cout << "  A network manager wants to find the cheapest route that\n";
    cout << "  audits all " << g.V << " routers and returns to the source.\n\n";

    cout << "  Brute-force cost: " << g.V << "! = ";
    long long fact = 1;
    for (int i = 1; i <= g.V; ++i) {
        fact *= i;
        if (fact > 1e15) { cout << "> 1 quadrillion"; break; }
    }
    cout << " routes to check\n\n";

    // Greedy nearest-neighbour heuristic (not optimal but practical)
    cout << "  Greedy nearest-neighbour heuristic (not optimal):\n";
    vector<bool> visited(g.V, false);
    int cur = 0, total = 0;
    vector<int> tour = {cur};
    visited[cur] = true;

    for (int step = 1; step < g.V; ++step) {
        int best = -1, bestW = INF;
        for (auto [v, w] : g.adj[cur])
            if (!visited[v] && w < bestW) { bestW = w; best = v; }
        if (best == -1) {
            // try any unvisited node
            for (int i = 0; i < g.V; ++i)
                if (!visited[i]) { best = i; bestW = INF; break; }
        }
        if (best == -1) break;
        visited[best] = true;
        total += bestW;
        tour.push_back(best);
        cur = best;
    }

    cout << "  Tour: ";
    for (int i = 0; i < (int)tour.size(); ++i) {
        if (i) cout << " -> ";
        cout << g.name(tour[i]);
    }
    cout << "\n  Approx. cost: " << total << "ms  (heuristic, NOT guaranteed optimal)\n\n";

    cout << "  WHY WE USE DIJKSTRA INSTEAD\n";
    printSep();
    cout << "  TSP (visit all routers once)  : NP-Hard  -- no fast exact solution\n";
    cout << "  Dijkstra (source to one dest) : P        -- O((V+E) log V)\n\n";
    cout << "  In practice, routers only need SINGLE-DESTINATION paths,\n";
    cout << "  not tours visiting every node. This is why Dijkstra and\n";
    cout << "  Bellman-Ford are used in real protocols (OSPF, BGP).\n";
    cout << "\n  >> NP-completeness justifies why we use greedy heuristics\n";
    cout << "     instead of exhaustive optimal solutions in real networks.\n";
}

// ============================================================
//  MAIN
// ============================================================
int main() {
    printHeader("NETWORK TRAFFIC OPTIMIZER -- LOOKUP & INSPECTION");
    cout << "  Member 4  |  Unit 5  --  Hashing, String Matching, NP\n";

    Graph g = buildNetwork();

    demoChaining();
    demoOpenAddr();
    naiveMatch();
    kmpMatch();
    rabinKarp();
    npDiscussion(g);

    cout << "\n[DONE] All lookup and inspection algorithms completed.\n\n";
    return 0;
}
