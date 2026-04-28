/*
============================================================
  traffic_optimizer.cpp  --  Member 3  |  Unit 4
  Network Traffic Optimizer
============================================================
  Compile:
    g++ -std=c++11 -O2 -o traffic_optimizer traffic_optimizer.cpp
  Run:
    ./traffic_optimizer

  Algorithms (relevant to traffic/bandwidth optimization):
    1. Activity Selection  -- O(n log n)  Greedy
    2. Job Sequencing      -- O(n log n)  Greedy
    3. Fractional Knapsack -- O(n log n)  Greedy
    4. 0/1 Knapsack        -- O(n * W)    DP
    5. Fibonacci           -- O(n)        Memoization vs Tabulation
    6. Huffman Codes       -- O(n log n)  Greedy
============================================================
*/

#include "graph.h"

// ============================================================
//  1. ACTIVITY SELECTION  (Unit 4)  [GREEDY]
//  Applied: select max number of non-overlapping time slots
//           for transmitting data bursts on a shared link
// ============================================================
struct Activity {
    int id, start, finish, bandwidth; // bandwidth in Mbps
};

void activitySelection() {
    printHeader("1. Activity Selection -- Max Non-Overlapping Transmissions");
    printExplain(
        "ACTIVITY SELECTION (Greedy)",
        "Select the maximum number of non-overlapping transmissions\n"
        "  on a shared link. Greedily pick the earliest-finishing\n"
        "  activity that starts after the last selected one ends.",
        "O(n log n) to sort by finish time, then O(n) greedy pass"
    );

    vector<Activity> acts = {
        {1, 0,  3, 10}, {2, 1,  4, 20}, {3, 2,  5, 15},
        {4, 3,  6, 25}, {5, 5,  8, 30}, {6, 6,  9, 20},
        {7, 7, 11, 35}, {8, 8, 12, 10}, {9,10, 13, 40},
    };

    sort(acts.begin(), acts.end(),
         [](const Activity& a, const Activity& b){ return a.finish < b.finish; });

    cout << "  All available transmission slots:\n";
    cout << "  " << left << setw(5) << "ID"
         << setw(10) << "Start(ms)"
         << setw(12) << "Finish(ms)"
         << "Bandwidth(Mbps)\n";
    printSep();
    for (auto& a : acts)
        cout << "  " << setw(5) << a.id << setw(10) << a.start
             << setw(12) << a.finish << a.bandwidth << "\n";

    vector<Activity> selected;
    int lastFinish = -1;
    for (auto& a : acts) {
        if (a.start >= lastFinish) {
            selected.push_back(a);
            lastFinish = a.finish;
        }
    }

    cout << "\n  Selected (non-overlapping, max count):\n";
    int totalBW = 0;
    for (auto& a : selected) {
        cout << "  Slot " << a.id << ": [" << a.start << "ms - "
             << a.finish << "ms]  " << a.bandwidth << "Mbps\n";
        totalBW += a.bandwidth;
    }
    cout << "\n  Total selected: " << selected.size() << " slots\n";
    cout << "  Total bandwidth delivered: " << totalBW << " Mbps\n";
    cout << "\n  >> Greedy works here because sorting by finish time\n"
         << "     always leaves maximum room for future selections.\n";
}

// ============================================================
//  2. JOB SEQUENCING  (Unit 4)  [GREEDY]
//  Applied: schedule packets to maximise on-time deliveries
//           each packet has a deadline and a profit (QoS score)
// ============================================================
struct Job {
    int id, deadline, profit;
};

void jobSequencing() {
    printHeader("2. Job Sequencing -- Maximise On-Time Packet Delivery");
    printExplain(
        "JOB SEQUENCING (Greedy)",
        "Each packet has a deadline (must be sent by slot N) and a\n"
        "  profit (QoS reward for delivering on time). Greedily assign\n"
        "  highest-profit packets to the latest available slot <= deadline.",
        "O(n log n) sort + O(n * max_deadline) slot assignment"
    );

    vector<Job> jobs = {
        {1,2,100},{2,1,60},{3,2,80},{4,1,40},
        {5,3,120},{6,3,90},{7,2,70},{8,4,110},
    };
    sort(jobs.begin(), jobs.end(),
         [](const Job& a, const Job& b){ return a.profit > b.profit; });

    int maxD = 0;
    for (auto& j : jobs) maxD = max(maxD, j.deadline);
    vector<int> slot(maxD+1, -1);

    int totalProfit = 0;
    vector<Job> scheduled;
    for (auto& j : jobs) {
        for (int s = min(j.deadline, maxD); s >= 1; s--) {
            if (slot[s] == -1) {
                slot[s] = j.id;
                scheduled.push_back(j);
                totalProfit += j.profit;
                break;
            }
        }
    }

    sort(scheduled.begin(), scheduled.end(),
         [](const Job& a, const Job& b){ return a.deadline < b.deadline; });

    cout << "  Scheduled packets (in deadline order):\n";
    cout << "  " << left << setw(8) << "Slot"
         << setw(10) << "Packet" << "QoS Profit\n";
    printSep();
    for (int s = 1; s <= maxD; ++s)
        if (slot[s] != -1) {
            for (auto& j : scheduled)
                if (j.id == slot[s])
                    cout << "  " << setw(8) << s << setw(10) << j.id
                         << j.profit << "\n";
        }
    cout << "\n  Total QoS profit: " << totalProfit << "\n";
    cout << "\n  >> Packets with highest profit are scheduled first,\n"
         << "     maximising network QoS score within deadlines.\n";
}

// ============================================================
//  3. FRACTIONAL KNAPSACK  (Unit 4)  [GREEDY]
//  Applied: allocate limited bandwidth to maximize throughput
// ============================================================
struct BandwidthRequest {
    int id;
    double value;     // throughput gain (Mbps)
    double weight;    // bandwidth required (Mbps)
    double ratio() const { return value / weight; }
};

void fractionalKnapsack() {
    printHeader("3. Fractional Knapsack -- Bandwidth Allocation (Greedy)");
    printExplain(
        "FRACTIONAL KNAPSACK (Greedy)",
        "A link has limited capacity. Each flow requests bandwidth and\n"
        "  offers a throughput gain. Greedily take flows in order of\n"
        "  value/weight ratio. Fractions allowed (partial bandwidth).",
        "O(n log n) to sort by ratio, then O(n) greedy selection"
    );

    vector<BandwidthRequest> flows = {
        {1, 60, 10}, {2, 100, 20}, {3, 120, 30},
        {4, 80,  15}, {5, 50,  5}, {6, 90,  25},
    };
    double capacity = 50.0;

    sort(flows.begin(), flows.end(),
         [](const BandwidthRequest& a, const BandwidthRequest& b){
             return a.ratio() > b.ratio();
         });

    cout << "  Available bandwidth capacity: " << capacity << " Mbps\n\n";
    cout << "  " << left << setw(8) << "Flow"
         << setw(12) << "Value"
         << setw(12) << "Required"
         << setw(12) << "Ratio"
         << setw(14) << "Allocated"
         << "Gain\n";
    printSep();

    double totalGain = 0;
    double remaining = capacity;
    for (auto& f : flows) {
        double alloc = min((double)f.weight, remaining);
        double gain = alloc * f.ratio();
        totalGain += gain;
        remaining -= alloc;
        cout << fixed << setprecision(1);
        cout << "  " << setw(8) << f.id
             << setw(12) << f.value
             << setw(12) << f.weight
             << setw(12) << f.ratio()
             << setw(14) << alloc
             << gain << "\n";
        if (remaining <= 0) break;
    }
    cout << "\n  Total throughput gain: " << totalGain << " Mbps\n";
    cout << "\n  >> Fractional knapsack maximises bandwidth utilization.\n"
         << "     Greedy works because partial allocation is allowed.\n";
}

// ============================================================
//  4. 0/1 KNAPSACK  (Unit 4)  [DYNAMIC PROGRAMMING]
//  Applied: allocate router memory slots to packet buffers
//           (cannot split -- either allocate fully or not at all)
// ============================================================
void knapsack01() {
    printHeader("4. 0/1 Knapsack -- Router Buffer Allocation (DP)");
    printExplain(
        "0/1 KNAPSACK (Dynamic Programming)",
        "Unlike fractional knapsack, here items cannot be split.\n"
        "  Either a buffer is fully allocated or not at all.\n"
        "  DP fills a table: dp[i][w] = max value using first i items\n"
        "  with weight limit w.",
        "O(n * W) time and space  --  DP tabulation"
    );

    // Buffer slots: value = throughput improvement, weight = memory (KB)
    vector<pair<int,int>> items = { // {value, weight}
        {60,10},{100,20},{120,30},{80,15},{50,5},{90,25},{70,18}
    };
    int W = 50; // router has 50 KB buffer space
    int n = items.size();

    vector<vector<int>> dp(n+1, vector<int>(W+1, 0));
    for (int i = 1; i <= n; ++i)
        for (int w = 0; w <= W; ++w) {
            dp[i][w] = dp[i-1][w];
            if (items[i-1].second <= w)
                dp[i][w] = max(dp[i][w],
                               dp[i-1][w - items[i-1].second] + items[i-1].first);
        }

    // Traceback
    vector<int> chosen;
    int w = W;
    for (int i = n; i > 0; i--)
        if (dp[i][w] != dp[i-1][w]) { chosen.push_back(i); w -= items[i-1].second; }
    reverse(chosen.begin(), chosen.end());

    cout << "  Router buffer limit: " << W << " KB\n\n";
    cout << "  Available buffer types:\n";
    cout << "  " << left << setw(8) << "Buffer"
         << setw(16) << "Value(throughput)"
         << "Weight(KB)\n";
    printSep();
    for (int i = 0; i < n; ++i)
        cout << "  " << setw(8) << i+1 << setw(16) << items[i].first
             << items[i].second << "\n";

    cout << "\n  Optimal selection (DP):\n";
    int totalVal = 0, totalWt = 0;
    for (int idx : chosen) {
        cout << "  Buffer " << idx << ": value=" << items[idx-1].first
             << "  weight=" << items[idx-1].second << " KB\n";
        totalVal += items[idx-1].first; totalWt += items[idx-1].second;
    }
    cout << "\n  Total value: " << totalVal
         << "  Total weight: " << totalWt << "/" << W << " KB\n";
    cout << "\n  >> 0/1 knapsack is optimal but greedy fails here.\n"
         << "     DP considers ALL combinations implicitly.\n";
}

// ============================================================
//  5. FIBONACCI -- Memoization vs Tabulation  (Unit 4)
//  Applied: demonstrate overlapping subproblems concept
//           (underlying principle of all DP algorithms above)
// ============================================================
map<int,long long> memo;
long long fibMemo(int n) {
    if (n <= 1) return n;
    if (memo.count(n)) return memo[n];
    return memo[n] = fibMemo(n-1) + fibMemo(n-2);
}

long long fibTab(int n) {
    if (n <= 1) return n;
    vector<long long> dp(n+1);
    dp[0]=0; dp[1]=1;
    for (int i = 2; i <= n; ++i) dp[i] = dp[i-1] + dp[i-2];
    return dp[n];
}

void fibonacciComparison() {
    printHeader("5. Fibonacci -- Memoization vs Tabulation");
    printExplain(
        "MEMOIZATION vs TABULATION",
        "Both avoid redundant computation (overlapping subproblems).\n"
        "  Memoization = top-down (recursive + cache).\n"
        "  Tabulation  = bottom-up (iterative, fills table from base case).\n"
        "  This is the core concept behind ALL DP algorithms in this project.",
        "Both O(n) time  |  Memoization O(n) call stack  |  Tabulation O(n) array"
    );

    cout << "  " << left << setw(6) << "n"
         << setw(22) << "Memoization"
         << "Tabulation\n";
    printSep();
    for (int i : {1,5,10,15,20,30,40}) {
        memo.clear();
        long long m = fibMemo(i);
        long long t = fibTab(i);
        cout << "  " << setw(6) << i << setw(22) << m << t << "\n";
    }
    cout << "\n  Key difference:\n";
    cout << "    Memoization: only computes values that are actually needed\n";
    cout << "    Tabulation : always fills entire table (safer, no stack overflow)\n";
    cout << "\n  >> Use tabulation for 0/1 Knapsack (large n).\n"
         << "     Use memoization when only a few subproblems are needed.\n";
}

// ============================================================
//  6. HUFFMAN CODES  (Unit 4)  [GREEDY]
//  Applied: compress packet headers to reduce bandwidth usage
// ============================================================
struct HuffNode {
    char ch; int freq;
    HuffNode *left, *right;
    HuffNode(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

struct HuffCmp {
    bool operator()(HuffNode* a, HuffNode* b) { return a->freq > b->freq; }
};

void buildCodes(HuffNode* node, const string& code,
                map<char,string>& codes) {
    if (!node) return;
    if (!node->left && !node->right) { codes[node->ch] = code; return; }
    buildCodes(node->left,  code + "0", codes);
    buildCodes(node->right, code + "1", codes);
}

void huffmanCodes() {
    printHeader("6. Huffman Codes -- Header Compression");
    printExplain(
        "HUFFMAN CODING (Greedy)",
        "Assigns shorter bit codes to more frequent symbols and longer\n"
        "  codes to rare ones. Applied here to compress packet protocol\n"
        "  headers. Greedily merges two lowest-frequency nodes each step.",
        "O(n log n)  --  min-heap priority queue"
    );

    // Packet protocol field frequencies in a typical traffic sample
    vector<pair<char,int>> freq = {
        {'T',45},{'U',25},{'I',15},{'A',10},{'B',5}
        //  TCP     UDP     ICMP    ACK     BGP
    };

    cout << "  Protocol field frequencies in captured traffic:\n";
    cout << "  T=TCP(" << 45 << ")  U=UDP(" << 25 << ")  "
         << "I=ICMP(" << 15 << ")  A=ACK(" << 10 << ")  B=BGP(" << 5 << ")\n\n";

    priority_queue<HuffNode*, vector<HuffNode*>, HuffCmp> pq;
    
    // C++11 FIX: Replaced auto [c, f] with standard pair access
    for (auto& p : freq) pq.push(new HuffNode(p.first, p.second));

    while (pq.size() > 1) {
        HuffNode* l = pq.top(); pq.pop();
        HuffNode* r = pq.top(); pq.pop();
        HuffNode* merged = new HuffNode('\0', l->freq + r->freq);
        merged->left = l; merged->right = r;
        pq.push(merged);
    }

    map<char,string> codes;
    buildCodes(pq.top(), "", codes);

    map<char,string> names = {
        {'T',"TCP"},{'U',"UDP"},{'I',"ICMP"},{'A',"ACK"},{'B',"BGP"}
    };

    cout << "  " << left << setw(8) << "Proto"
         << setw(10) << "Freq"
         << setw(16) << "Huffman Code"
         << "Bits saved\n";
    printSep();

    int origBits = 0, compBits = 0;
    
    // C++11 FIX: Replaced auto [c, f] with standard pair access
    for (auto& p : freq) {
        char c = p.first;
        int f = p.second;
        int orig = 3; 
        int comp = codes[c].size();
        origBits += f * orig; compBits += f * comp;
        cout << "  " << setw(8) << names[c]
             << setw(10) << f
             << setw(16) << codes[c]
             << orig << " bits -> " << comp << " bits\n";
    }
    
    cout << "\n  Original encoding  : " << origBits << " bits\n";
    cout << "  Huffman encoding   : " << compBits << " bits\n";
    cout << fixed << setprecision(1);
    cout << "  Compression ratio  : "
         << (1.0 - (double)compBits/origBits) * 100 << "% reduction\n";
    cout << "\n  >> Huffman coding reduces header overhead on every packet.\n"
         << "     More frequent protocols get shorter codes.\n";
}

// ============================================================
//  MAIN
// ============================================================
int main() {
    printHeader("NETWORK TRAFFIC OPTIMIZER -- TRAFFIC OPTIMIZER");
    cout << "  Member 3  |  Unit 4  --  Greedy & Dynamic Programming\n";

    activitySelection();
    jobSequencing();
    fractionalKnapsack();
    knapsack01();
    fibonacciComparison();
    huffmanCodes();

    cout << "\n[DONE] All optimization algorithms completed.\n\n";
    return 0;
}
