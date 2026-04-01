/*
============================================================
  packet_scheduler.cpp  --  Member 2  |  Unit 2
  Network Traffic Optimizer
============================================================
  Compile:
    g++ -std=c++17 -O2 -o packet_scheduler packet_scheduler.cpp
  Run:
    ./packet_scheduler

  Algorithms (only those useful for packet scheduling):
    1. Heap Sort        -- O(n log n)   priority queue basis
    2. Merge Sort       -- O(n log n)   stable, good for packets
    3. Quick Sort       -- O(n log n)   fast in practice
    4. Counting Sort    -- O(n + k)     priority levels (1-5)
    5. Insertion Sort   -- O(n^2)       good for nearly-sorted
    6. Selection Sort   -- O(n^2)       for comparison
    7. Bubble Sort      -- O(n^2)       for comparison / teaching

  Plus: sorting comparison table and scheduling simulation
============================================================
*/

#include "graph.h"

// ============================================================
//  DISPLAY HELPERS
// ============================================================
void printPackets(const vector<Packet>& pkts, const string& label) {
    printSubHeader(label);
    printPacketHeader();
    for (auto& p : pkts) printPacketRow(p);
}

// ============================================================
//  1. HEAP SORT  (Unit 2)
//  Applied: schedules packets by priority using a max-heap
// ============================================================
void heapify(vector<Packet>& arr, int n, int i) {
    int largest = i, l = 2*i+1, r = 2*i+2;
    if (l < n && arr[l].priority < arr[largest].priority) largest = l;
    if (r < n && arr[r].priority < arr[largest].priority) largest = r;
    if (largest != i) { swap(arr[i], arr[largest]); heapify(arr, n, largest); }
}

vector<Packet> heapSort(vector<Packet> arr) {
    int n = arr.size();
    for (int i = n/2 - 1; i >= 0; i--) heapify(arr, n, i);
    for (int i = n-1; i > 0; i--) { swap(arr[0], arr[i]); heapify(arr, i, 0); }
    return arr;
}

void demoHeapSort(vector<Packet> pkts) {
    printHeader("1. Heap Sort -- Priority Queue Scheduling");
    printExplain(
        "HEAP SORT",
        "Builds a max-heap (or min-heap) from the data, then extracts\n"
        "  elements one by one. Each extraction takes O(log n).\n"
        "  Applied: packets with priority 1 (most urgent) served first.",
        "O(n log n) time  |  O(1) space  |  NOT stable  |  In-place"
    );
    auto sorted = heapSort(pkts);
    printPackets(sorted, "Packets sorted by priority (heap sort)");
    cout << "\n  >> Heap sort is the basis of real network schedulers.\n"
         << "     Priority 1 = real-time (VoIP), 5 = background downloads.\n";
}

// ============================================================
//  2. MERGE SORT  (Unit 2)
//  Applied: stable sort -- keeps arrival order for equal-priority packets
// ============================================================
void mergeHelper(vector<Packet>& arr, int l, int m, int r) {
    vector<Packet> left(arr.begin()+l, arr.begin()+m+1);
    vector<Packet> right(arr.begin()+m+1, arr.begin()+r+1);
    int i=0, j=0, k=l;
    while (i<(int)left.size() && j<(int)right.size())
        arr[k++] = (left[i] < right[j]) ? left[i++] : right[j++];
    while (i<(int)left.size())  arr[k++] = left[i++];
    while (j<(int)right.size()) arr[k++] = right[j++];
}

void mergeSortHelper(vector<Packet>& arr, int l, int r) {
    if (l >= r) return;
    int m = l + (r-l)/2;
    mergeSortHelper(arr, l, m);
    mergeSortHelper(arr, m+1, r);
    mergeHelper(arr, l, m, r);
}

vector<Packet> mergeSort(vector<Packet> arr) {
    mergeSortHelper(arr, 0, (int)arr.size()-1);
    return arr;
}

void demoMergeSort(vector<Packet> pkts) {
    printHeader("2. Merge Sort -- Stable Packet Ordering");
    printExplain(
        "MERGE SORT",
        "Divides the list in half recursively, then merges sorted halves.\n"
        "  STABLE: packets with equal priority keep their arrival order.\n"
        "  Important in networking -- earlier-arrived packets served first.",
        "O(n log n) time  |  O(n) space  |  STABLE  |  NOT in-place"
    );
    auto sorted = mergeSort(pkts);
    printPackets(sorted, "Packets sorted by priority (merge sort)");
    cout << "\n  >> Merge sort's stability guarantees fairness:\n"
         << "     two packets with the same priority are served\n"
         << "     in the order they arrived.\n";
}

// ============================================================
//  3. QUICK SORT  (Unit 2)
//  Applied: fast in-place sorting when order of equals doesn't matter
// ============================================================
int partition(vector<Packet>& arr, int lo, int hi) {
    int pivot = arr[hi].priority, i = lo - 1;
    for (int j = lo; j < hi; j++)
        if (arr[j].priority <= pivot) swap(arr[++i], arr[j]);
    swap(arr[i+1], arr[hi]);
    return i+1;
}

// Randomized quick sort (Unit 2 topic)
int randPartition(vector<Packet>& arr, int lo, int hi) {
    int r = lo + rand() % (hi - lo + 1);
    swap(arr[r], arr[hi]);
    return partition(arr, lo, hi);
}

void quickSortHelper(vector<Packet>& arr, int lo, int hi, bool randomized) {
    if (lo >= hi) return;
    int p = randomized ? randPartition(arr, lo, hi) : partition(arr, lo, hi);
    quickSortHelper(arr, lo, p-1, randomized);
    quickSortHelper(arr, p+1, hi, randomized);
}

vector<Packet> quickSort(vector<Packet> arr, bool randomized = false) {
    quickSortHelper(arr, 0, (int)arr.size()-1, randomized);
    return arr;
}

void demoQuickSort(vector<Packet> pkts) {
    printHeader("3. Quick Sort & Randomized Quick Sort");
    printExplain(
        "QUICK SORT / RANDOMIZED QUICK SORT",
        "Picks a pivot, partitions smaller elements left and larger right,\n"
        "  recurses on both halves. Randomized version picks a random pivot\n"
        "  to avoid worst-case O(n^2) on already-sorted input.",
        "Avg O(n log n) | Worst O(n^2) | O(log n) space | NOT stable"
    );
    auto s1 = quickSort(pkts, false);
    auto s2 = quickSort(pkts, true);
    printPackets(s1, "Quick sort result");
    printPackets(s2, "Randomized quick sort result");
    cout << "\n  >> Randomized quick sort avoids the O(n^2) worst case.\n"
         << "     Use when you cannot guarantee input is unsorted.\n";
}

// ============================================================
//  4. COUNTING SORT  (Unit 2)
//  Applied: sort by priority levels 1-5, linear time
// ============================================================
vector<Packet> countingSort(vector<Packet> arr) {
    int maxPri = 5;
    vector<vector<Packet>> buckets(maxPri + 1);
    for (auto& p : arr) buckets[p.priority].push_back(p);
    vector<Packet> res;
    for (int i = 1; i <= maxPri; ++i)
        for (auto& p : buckets[i]) res.push_back(p);
    return res;
}

void demoCountingSort(vector<Packet> pkts) {
    printHeader("4. Counting Sort -- Linear Time by Priority Level");
    printExplain(
        "COUNTING SORT",
        "Works when keys are integers in a known range (here: 1-5).\n"
        "  Counts how many packets have each priority, then places them\n"
        "  directly into the correct position -- no comparisons needed.",
        "O(n + k) time  |  O(k) space  |  STABLE  |  k = priority range (5)"
    );
    auto sorted = countingSort(pkts);
    printPackets(sorted, "Packets sorted by counting sort (priority 1-5)");
    cout << "\n  >> Fastest sort when key range is small and known.\n"
         << "     Perfect for priority-based scheduling (only 5 levels).\n";
}

// ============================================================
//  5. INSERTION SORT  (Unit 2)
//  Applied: online sort -- sort packets as they arrive one by one
// ============================================================
vector<Packet> insertionSort(vector<Packet> arr) {
    for (int i = 1; i < (int)arr.size(); ++i) {
        Packet key = arr[i]; int j = i-1;
        while (j >= 0 && arr[j].priority > key.priority)
            { arr[j+1] = arr[j]; j--; }
        arr[j+1] = key;
    }
    return arr;
}

void demoInsertionSort(vector<Packet> pkts) {
    printHeader("5. Insertion Sort -- Online / Nearly Sorted");
    printExplain(
        "INSERTION SORT",
        "Inserts each element into its correct position in the already-\n"
        "  sorted part. Efficient for ONLINE sorting (packets arriving\n"
        "  one by one) and nearly-sorted data.",
        "O(n^2) worst  |  O(n) best (sorted input)  |  STABLE  |  In-place"
    );
    // Demo online: show first 5 packets inserting one by one
    cout << "  Online insertion (first 6 packets arriving one by one):\n\n";
    vector<Packet> live;
    for (int i = 0; i < min(6, (int)pkts.size()); ++i) {
        live.push_back(pkts[i]);
        auto sorted = insertionSort(live);
        cout << "  After packet " << pkts[i].id
             << " (pri=" << pkts[i].priority << ") arrives: ";
        for (auto& p : sorted) cout << "P" << p.id << "(pri" << p.priority << ") ";
        cout << "\n";
    }
    cout << "\n  >> Insertion sort handles live packet streams efficiently\n"
         << "     when the queue is nearly sorted at all times.\n";
}

// ============================================================
//  6. SELECTION SORT  (Unit 2)  -- comparison / teaching
// ============================================================
vector<Packet> selectionSort(vector<Packet> arr) {
    int n = arr.size();
    for (int i = 0; i < n-1; ++i) {
        int minIdx = i;
        for (int j = i+1; j < n; ++j)
            if (arr[j].priority < arr[minIdx].priority) minIdx = j;
        swap(arr[i], arr[minIdx]);
    }
    return arr;
}

// ============================================================
//  7. BUBBLE SORT  (Unit 2)  -- comparison / teaching
// ============================================================
vector<Packet> bubbleSort(vector<Packet> arr) {
    int n = arr.size();
    for (int i = 0; i < n-1; ++i)
        for (int j = 0; j < n-i-1; ++j)
            if (arr[j].priority > arr[j+1].priority) swap(arr[j], arr[j+1]);
    return arr;
}

// ============================================================
//  6. SELECTION SORT DEMO  (Unit 2)
// ============================================================
void demoSelectionSort(vector<Packet> pkts) {
    printHeader("6. Selection Sort -- Minimum Swaps");
    printExplain(
        "SELECTION SORT",
        "Repeatedly finds the minimum-priority packet in the unsorted\n"
        "  portion and swaps it to the front. Makes exactly n-1 swaps\n"
        "  regardless of input -- useful when writes/swaps are expensive.",
        "O(n^2) time  |  O(1) space  |  NOT stable  |  In-place\n"
        "  Best case = worst case (always scans full unsorted range)"
    );
    int swaps = 0, comps = 0;
    vector<Packet> arr = pkts;
    int n = arr.size();
    for (int i = 0; i < n-1; ++i) {
        int minIdx = i;
        for (int j = i+1; j < n; ++j) { comps++; if (arr[j].priority < arr[minIdx].priority) minIdx = j; }
        if (minIdx != i) { swap(arr[i], arr[minIdx]); swaps++; }
    }
    printPackets(arr, "Packets sorted by selection sort");
    cout << "  Comparisons: " << comps << "  |  Swaps: " << swaps << "\n";
    cout << "\n  >> Selection sort always makes O(n^2) comparisons\n"
         << "     but only O(n) swaps -- good when swaps are costly.\n";
}

// ============================================================
//  7. BUBBLE SORT DEMO  (Unit 2)
// ============================================================
void demoBubbleSort(vector<Packet> pkts) {
    printHeader("7. Bubble Sort -- Adjacent Swap");
    printExplain(
        "BUBBLE SORT",
        "Repeatedly compares adjacent packets and swaps them if out\n"
        "  of order. Highest-priority packets 'bubble' to the front.\n"
        "  Optimised with an early-exit flag: stops if no swaps occur\n"
        "  in a full pass (already sorted).",
        "O(n^2) worst  |  O(n) best (sorted input)  |  STABLE  |  In-place"
    );
    int swaps = 0, comps = 0, passes = 0;
    vector<Packet> arr = pkts;
    int n = arr.size();
    for (int i = 0; i < n-1; ++i) {
        bool swapped = false; passes++;
        for (int j = 0; j < n-i-1; ++j) {
            comps++;
            if (arr[j].priority > arr[j+1].priority) {
                swap(arr[j], arr[j+1]); swaps++; swapped = true;
            }
        }
        if (!swapped) { cout << "  Early exit at pass " << passes << " (no swaps)\n"; break; }
    }
    printPackets(arr, "Packets sorted by bubble sort");
    cout << "  Passes: " << passes << "  |  Comparisons: " << comps
         << "  |  Swaps: " << swaps << "\n";
    cout << "\n  >> Bubble sort is rarely used in practice but clearly\n"
         << "     shows the concept of comparison-based sorting.\n";
}

// ============================================================
//  SORTING ALGORITHM COMPARISON TABLE  (Unit 2 requirement)
// ============================================================
void printSortComparison() {
    printHeader("Sorting Algorithm Comparison Table");
    cout << "\n  " << left
         << setw(20) << "Algorithm"
         << setw(16) << "Best"
         << setw(16) << "Average"
         << setw(16) << "Worst"
         << setw(10) << "Space"
         << setw(10) << "Stable"
         << "In-Place\n";
    printSep();
    auto r = [](const string& a, const string& b, const string& av,
                const string& w, const string& sp,
                const string& st, const string& ip) {
        cout << "  " << left << setw(20) << a << setw(16) << b
             << setw(16) << av << setw(16) << w
             << setw(10) << sp << setw(10) << st << ip << "\n";
    };
    r("Heap Sort",    "O(n log n)", "O(n log n)", "O(n log n)", "O(1)", "No",  "Yes");
    r("Merge Sort",   "O(n log n)", "O(n log n)", "O(n log n)", "O(n)", "Yes", "No");
    r("Quick Sort",   "O(n log n)", "O(n log n)", "O(n^2)",     "O(log n)", "No", "Yes");
    r("Rand. Quick",  "O(n log n)", "O(n log n)", "O(n log n)", "O(log n)", "No", "Yes");
    r("Counting Sort","O(n+k)",     "O(n+k)",     "O(n+k)",     "O(k)", "Yes", "No");
    r("Insertion Sort","O(n)",      "O(n^2)",     "O(n^2)",     "O(1)", "Yes", "Yes");
    r("Selection Sort","O(n^2)",    "O(n^2)",     "O(n^2)",     "O(1)", "No",  "Yes");
    r("Bubble Sort",  "O(n)",       "O(n^2)",     "O(n^2)",     "O(1)", "Yes", "Yes");

    cout << "\n  Best choice for packet scheduling:\n";
    cout << "    - Real-time priority scheduling : Heap Sort (O(n log n), in-place)\n";
    cout << "    - Stable ordering by arrival    : Merge Sort (preserves FIFO)\n";
    cout << "    - Fixed priority levels 1-5     : Counting Sort (fastest, O(n+k))\n";
    cout << "    - Live packet stream            : Insertion Sort (O(n) when nearly sorted)\n";
}

// ============================================================
//  SCHEDULING SIMULATION
//  Shows how sorted order affects total latency
// ============================================================
void schedulingSimulation(const vector<Packet>& pkts) {
    printHeader("Packet Scheduling Simulation");
    cout << "\n  Simulating transmission of 10 packets.\n";
    cout << "  Each packet takes (priority * 10)ms to process.\n\n";

    auto sim = [&](vector<Packet> sorted, const string& label) {
        int time = 0, missedDeadlines = 0;
        cout << "  Strategy: " << label << "\n";
        cout << "  " << left << setw(5) << "P#"
             << setw(8) << "Pri" << setw(12) << "Start"
             << setw(12) << "Finish" << "Status\n";
        cout << "  " << string(48, '-') << "\n";
        for (auto& p : sorted) {
            int start = time;
            int finish = time + p.priority * 10;
            string status = (finish <= p.deadline) ? "OK" : "LATE";
            if (status == "LATE") missedDeadlines++;
            cout << "  " << setw(5) << p.id
                 << setw(8) << p.priority
                 << setw(12) << start
                 << setw(12) << finish
                 << status << "\n";
            time = finish;
        }
        cout << "  Missed deadlines: " << missedDeadlines << "/10\n\n";
    };

    auto first10 = vector<Packet>(pkts.begin(), pkts.begin()+10);
    sim(first10, "FIFO (no sorting)");
    sim(countingSort(first10), "Priority-sorted (counting sort)");
    cout << "  >> Sorting by priority reduces missed deadlines significantly.\n";
}

// ============================================================
//  MAIN
// ============================================================
int main() {
    printHeader("NETWORK TRAFFIC OPTIMIZER -- PACKET SCHEDULER");
    cout << "  Member 2  |  Unit 2  --  Sorting Algorithms\n";

    vector<Packet> pkts = buildTestPackets();
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

    cout << "\n[DONE] All sorting algorithms completed.\n\n";
    return 0;
}
