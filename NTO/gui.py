import tkinter as tk
from tkinter import ttk, scrolledtext, filedialog, messagebox
import subprocess
import os
import re

# Try to import visualization libraries
try:
    import networkx as nx
    from matplotlib.figure import Figure
    from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
    HAS_VISUALS = True
except ImportError:
    HAS_VISUALS = False

class NetworkOptimizerGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("Network Traffic Optimizer - Visual Dashboard")
        self.root.geometry("1100x750")
        
        self.filepath = "network.txt"
        self.current_nodes = []
        self.current_edges = []
        
        self.setup_data()
        self.setup_ui()
        self.update_graph_view() # Draw initial demo graph

    def setup_data(self):
        self.modules = {
            1: "Routing Engine (Shortest paths, Loops, Backbones)",
            2: "Packet Scheduler (Priority Sorting)",
            3: "Traffic Optimizer (Bandwidth & DP Allocation)",
            4: "Lookup & Inspection (Hashing, String Matching)"
        }
        self.algorithms = {
            1: [(1, "Dijkstra's (Lowest latency)"), (2, "Bellman-Ford (Handles negative weights)"),
                (3, "BFS (Fewest hops)"), (4, "DFS & Cycle Detection"),
                (5, "Topological Sort"), (6, "Floyd-Warshall (All pairs)"),
                (7, "Kruskal's / Prim's MST (Backbone)")],
            2: [(1, "Heap Sort"), (2, "Merge Sort (Stable)"), (3, "Quick Sort"),
                (4, "Counting Sort"), (5, "Scheduling Simulation")],
            3: [(1, "Activity Selection"), (2, "Job Sequencing"),
                (3, "Fractional Knapsack"), (4, "0/1 Knapsack DP"), (5, "Huffman Coding")],
            4: [(1, "Hash Tables - Chaining"), (2, "Hash Tables - Open Addr"),
                (3, "KMP Algorithm"), (4, "Rabin-Karp Algorithm"), (5, "NP-Completeness TSP")]
        }

    def setup_ui(self):
        # --- MAIN LAYOUT ---
        left_panel = tk.Frame(self.root, width=350)
        left_panel.pack(side="left", fill="y", padx=10, pady=10)
        
        right_panel = tk.Frame(self.root)
        right_panel.pack(side="right", fill="both", expand=True, padx=10, pady=10)

        # ================= LEFT PANEL (CONTROLS) =================
        # 1. Network Input
        net_frame = tk.LabelFrame(left_panel, text="1. Network Input Setup", padx=10, pady=10)
        net_frame.pack(fill="x", pady=(0, 10))
        
        self.net_choice = tk.IntVar(value=1)
        tk.Radiobutton(net_frame, text="Built-in 12-Node Demo", variable=self.net_choice, value=1, command=self.update_graph_view).pack(anchor="w")
        
        file_frame = tk.Frame(net_frame)
        file_frame.pack(fill="x", anchor="w")
        tk.Radiobutton(file_frame, text="Load from File:", variable=self.net_choice, value=2, command=self.update_graph_view).pack(side="left")
        self.btn_browse = tk.Button(file_frame, text="Browse", state="disabled", command=self.browse_file)
        self.btn_browse.pack(side="left", padx=5)
        
        tk.Radiobutton(net_frame, text="Interactive/Manual Input", variable=self.net_choice, value=3, command=self.update_graph_view).pack(anchor="w")
        self.txt_manual = tk.Text(net_frame, height=5, width=35, state="disabled", bg="#f0f0f0")
        self.txt_manual.pack(pady=5)
        self.txt_manual.insert("1.0", "4\nA B C D\n3\n0 1 10\n1 2 5\n2 3 20")

        # 2. Module & Algorithm
        mod_frame = tk.LabelFrame(left_panel, text="2. Select Module & Algorithm", padx=10, pady=10)
        mod_frame.pack(fill="x", pady=10)
        
        self.mod_choice = tk.IntVar(value=1)
        for val, desc in self.modules.items():
            tk.Radiobutton(mod_frame, text=desc, variable=self.mod_choice, value=val, command=self.update_algorithms).pack(anchor="w")

        self.alg_inner_frame = tk.Frame(mod_frame)
        self.alg_inner_frame.pack(fill="x", pady=5)
        self.alg_choice = tk.IntVar(value=1)
        self.update_algorithms()

        # 3. Action Buttons
        tk.Button(left_panel, text="1. Compile C++ Backend", command=self.compile_cpp, bg="#2196F3", fg="white", font=("Arial", 10, "bold")).pack(fill="x", pady=(10,5))
        tk.Button(left_panel, text="2. RUN ALGORITHM", command=self.run_optimizer, bg="#4CAF50", fg="white", font=("Arial", 12, "bold")).pack(fill="x")
        tk.Button(left_panel, text="Refresh Visual Graph", command=self.update_graph_view).pack(fill="x", pady=5)

        # ================= RIGHT PANEL (VISUALS & OUTPUT) =================
        # Top Right: Graph Visualization
        self.graph_frame = tk.LabelFrame(right_panel, text="Network Visualization", bg="white")
        self.graph_frame.pack(fill="both", expand=True, pady=(0, 10))
        
        if not HAS_VISUALS:
            tk.Label(self.graph_frame, text="Missing libraries! Run:\npip install networkx matplotlib", fg="red", font=("Arial", 14)).pack(expand=True)
            self.canvas_widget = None
        else:
            self.fig = Figure(figsize=(5, 4), dpi=100)
            self.ax = self.fig.add_subplot(111)
            self.canvas = FigureCanvasTkAgg(self.fig, master=self.graph_frame)
            self.canvas_widget = self.canvas.get_tk_widget()
            self.canvas_widget.pack(fill="both", expand=True)

        # Bottom Right: Console Output
        tk.Label(right_panel, text="C++ Algorithm Output:").pack(anchor="w")
        self.output = scrolledtext.ScrolledText(right_panel, height=12, bg="black", fg="#00FF00", font=("Consolas", 10))
        self.output.pack(fill="both")

    def toggle_inputs(self):
        choice = self.net_choice.get()
        self.btn_browse.config(state="normal" if choice == 2 else "disabled")
        self.txt_manual.config(state="normal" if choice == 3 else "disabled", bg="white" if choice == 3 else "#f0f0f0")

    def browse_file(self):
        filename = filedialog.askopenfilename(filetypes=[("Text Files", "*.txt")])
        if filename:
            self.filepath = filename
            self.update_graph_view()

    def update_algorithms(self):
        for widget in self.alg_inner_frame.winfo_children(): widget.destroy()
        self.alg_choice.set(1)
        for val, desc in self.algorithms[self.mod_choice.get()]:
            tk.Radiobutton(self.alg_inner_frame, text=desc, variable=self.alg_choice, value=val).pack(anchor="w")

    # --- GRAPH PARSING & DRAWING LOGIC ---
    def parse_graph_data(self):
        self.current_nodes = []
        self.current_edges = []
        choice = self.net_choice.get()

        try:
            if choice == 1: # Demo
                self.current_nodes = ["R0","R1","R2","R3","R4","R5","R6","R7","R8","R9","R10","R11"]
                self.current_edges = [
                    (0,1,4), (1,2,8), (0,3,11), (1,3,5), (2,5,2), (2,4,3), (3,4,7),
                    (3,6,6), (4,5,9), (4,7,14), (5,8,7), (5,10,9), (6,7,3), (6,9,8),
                    (7,8,5), (7,9,2), (8,11,3), (9,10,4), (10,11,6)
                ]
            elif choice == 2: # File
                if not os.path.exists(self.filepath): return
                with open(self.filepath, 'r') as f:
                    lines = f.read().split()
                    if not lines: return
                    V = int(lines[0])
                    E = int(lines[1])
                    self.current_nodes = lines[2:2+V]
                    idx = 2+V
                    for _ in range(E):
                        self.current_edges.append((int(lines[idx]), int(lines[idx+1]), int(lines[idx+2])))
                        idx += 3
            elif choice == 3: # Manual
                lines = self.txt_manual.get("1.0", tk.END).split()
                if not lines: return
                V = int(lines[0])
                self.current_nodes = lines[1:1+V]
                E = int(lines[1+V])
                idx = 2+V
                for _ in range(E):
                    self.current_edges.append((int(lines[idx]), int(lines[idx+1]), int(lines[idx+2])))
                    idx += 3
        except Exception as e:
            print("Graph parse warning (incomplete data):", e)

    def draw_graph(self, highlight_edges=None):
        if not HAS_VISUALS: return
        self.ax.clear()
        
        if not self.current_nodes:
            self.ax.text(0.5, 0.5, "No valid network data provided.", ha='center', va='center')
            self.canvas.draw()
            return

        G = nx.DiGraph()
        for i, name in enumerate(self.current_nodes):
            G.add_node(i, label=name)
        for u, v, w in self.current_edges:
            G.add_edge(u, v, weight=w)

        pos = nx.spring_layout(G, seed=42) # Consistent layout
        labels = nx.get_node_attributes(G, 'label')
        edge_labels = nx.get_edge_attributes(G, 'weight')

        # Base drawing
        nx.draw_networkx_nodes(G, pos, ax=self.ax, node_color='lightblue', node_size=500)
        nx.draw_networkx_labels(G, pos, labels, ax=self.ax, font_size=9, font_weight="bold")
        nx.draw_networkx_edges(G, pos, ax=self.ax, edge_color='gray', arrows=True)
        nx.draw_networkx_edge_labels(G, pos, edge_labels=edge_labels, ax=self.ax, font_size=8)

        # Highlight algorithmic output
        if highlight_edges:
            nx.draw_networkx_edges(G, pos, edgelist=highlight_edges, edge_color='red', width=2.5, arrows=True)
            nx.draw_networkx_nodes(G, pos, nodelist=set(sum(highlight_edges, ())), node_color='salmon', node_size=600)

        self.ax.axis('off')
        self.canvas.draw()

    def update_graph_view(self):
        self.toggle_inputs()
        self.parse_graph_data()
        self.draw_graph()

    # --- EXECUTION ---
    def compile_cpp(self):
        self.output.delete(1.0, tk.END)
        self.output.insert(tk.END, "Compiling C++ backend...\n")
        self.root.update()
        try:
            subprocess.run(["g++", "-std=c++11", "-O2", "-o", "nto", "main.cpp"], check=True, stderr=subprocess.PIPE)
            self.output.insert(tk.END, "[SUCCESS] Compiled! Ready to run algorithms.\n")
        except subprocess.CalledProcessError as e:
            self.output.insert(tk.END, f"[ERROR]\n{e.stderr.decode()}\n")

    def run_optimizer(self):
        self.output.delete(1.0, tk.END)
        self.root.update()
        
        exe_name = "./nto" if os.name != "nt" else "nto.exe"
        if not os.path.exists(exe_name):
            self.output.insert(tk.END, "[!] Executable not found. Compile first.\n")
            return

        input_data = f"{self.net_choice.get()}\n"
        if self.net_choice.get() == 2: input_data += f"{self.filepath}\n"
        elif self.net_choice.get() == 3: input_data += self.txt_manual.get("1.0", tk.END).strip() + "\n"
        input_data += f"{self.mod_choice.get()}\n{self.alg_choice.get()}\n"

        try:
            process = subprocess.run([exe_name], input=input_data, text=True, capture_output=True, timeout=5)
            out_text = process.stdout
            self.output.insert(tk.END, out_text)
            
            # --- INTELLIGENT HIGHLIGHT PARSER ---
            # Finds paths like "R1 -> R2" or MST edge additions in the C++ output
            highlighted = []
            if self.mod_choice.get() == 1:
                # Find occurrences of Word -> Word
                matches = re.findall(r'(\w+)\s*->\s*(\w+)', out_text)
                for u_name, v_name in matches:
                    if u_name in self.current_nodes and v_name in self.current_nodes:
                        u_idx = self.current_nodes.index(u_name)
                        v_idx = self.current_nodes.index(v_name)
                        highlighted.append((u_idx, v_idx))
                
                # Update the visual graph with the computed paths/backbones!
                self.draw_graph(highlight_edges=highlighted)
                
        except Exception as e:
            self.output.insert(tk.END, f"\n[!] Error:\n{str(e)}\n")

if __name__ == "__main__":
    root = tk.Tk()
    app = NetworkOptimizerGUI(root)
    root.mainloop()
    
