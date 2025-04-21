# Orlando Pathfinding Visualizer

This project visualizes real-world pathfinding in Orlando using Djikstra's and A* algorithms. The C++ backend loads a real road network from `orlando_graph.csv`, computes shortest paths using both algorithms, and sends the results over TCP sockets. The Processing sketch (`.pde` file) renders the full map with interactive node selection, tourist landmarks, and visual path comparison.

# How to Use

1. **Processing Setup**
   - Download and install Processing as a **desktop application**, not the web editor.
   - Rename the dataset file `orlando_graph.csv` to `orlando_graph1.csv` and place it in the same directory as the `.pde` sketch.

2. **C++ Backend Setup**
   - Compile and run `main.cpp` in CLion (or another C++ IDE that supports Windows).
   - The backend will listen for socket connections on port `8081`.
   - After running main.cpp, run the Processing Sketch. 

3. **Interaction**
   - After both programs are running, click two nodes in the Processing window.
   - The shortest paths (Djikstra and A*) will be computed and visualized.
   - Press 'd' to show Djikstra's alone, and 'a' to show the A* path alone. 

## Notes

- **Port 8081** must be available and not in use by any other application.
- The C++ code uses **Windows-specific socket programming (Winsock API)**. It will not compile or run on macOS or Linux without adaptation.
