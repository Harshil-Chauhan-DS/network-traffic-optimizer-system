CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall

# Default: build the unified program and all individual modules
all: nto routing_engine packet_scheduler traffic_optimizer lookup_inspect
	@echo ""
	@echo "Build complete. Run with: ./nto"
	@echo ""

# Unified program (recommended -- runs all modules from one menu)
nto: main.cpp routing_engine.cpp packet_scheduler.cpp \
     traffic_optimizer.cpp lookup_inspect.cpp graph.h
	$(CXX) $(CXXFLAGS) -o nto main.cpp

# Individual modules (can also be run separately)
routing_engine: routing_engine.cpp graph.h
	$(CXX) $(CXXFLAGS) -o routing_engine routing_engine.cpp

packet_scheduler: packet_scheduler.cpp graph.h
	$(CXX) $(CXXFLAGS) -o packet_scheduler packet_scheduler.cpp

traffic_optimizer: traffic_optimizer.cpp graph.h
	$(CXX) $(CXXFLAGS) -o traffic_optimizer traffic_optimizer.cpp

lookup_inspect: lookup_inspect.cpp graph.h
	$(CXX) $(CXXFLAGS) -o lookup_inspect lookup_inspect.cpp

# Remove all compiled binaries
clean:
	rm -f nto routing_engine packet_scheduler traffic_optimizer lookup_inspect

.PHONY: all clean
