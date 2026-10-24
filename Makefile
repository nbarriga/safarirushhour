#CXXFLAGS = -O3 -msse -msse2 -Wall -Wextra -std=c++14 
CXXFLAGS = -g -Wall -Wextra -std=c++17
CXX=g++
OBJS = Common.o State.o TranspositionTable.o Puzzle.o 

LIBS = -lboost_program_options

# INSTRUMENTATION: stand-alone metrics driver (no boost needed).
# Compiled directly from the sources with optimisation and asserts disabled (-DNDEBUG).
METRICS_SRC = Common.cpp State.cpp TranspositionTable.cpp Puzzle.cpp SafariMetrics.cpp
metrics:	$(METRICS_SRC) Common.h State.h TranspositionTable.h Puzzle.h
	$(CXX) -O2 -DNDEBUG -std=c++17 -o SafariMetrics $(METRICS_SRC)

all:	safari verify

safari:	$(OBJS) Safari.o
	$(CXX) $(CXXFLAGS)  -o safari $(OBJS) Safari.o $(LIBS)

verify:	$(OBJS) Verify.o
	$(CXX) $(CXXFLAGS) -o verify $(OBJS) Verify.o $(LIBS)
clean:
	rm -f $(OBJS) safari verify safari.a SafariMetrics $(TARGET)

lib: $(OBJS)
	ar rvs safari.a $(OBJS)