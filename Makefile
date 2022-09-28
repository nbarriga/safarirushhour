#CXXFLAGS = -O3 -msse -msse2 -Wall -Wextra -std=c++14 
CXXFLAGS = -g -Wall -Wextra -std=c++14
CXX=g++
OBJS = Common.o State.o TranspositionTable.o Puzzle.o 

LIBS = -lboost_program_options

all:	safari verify

safari:	$(OBJS) Safari.o
	$(CXX) $(CXXFLAGS)  -o safari $(OBJS) Safari.o $(LIBS)

verify:	$(OBJS) Verify.o
	$(CXX) $(CXXFLAGS) -o verify $(OBJS) Verify.o $(LIBS)
clean:
	rm -f $(OBJS) safari verify safari.a $(TARGET)

lib: $(OBJS)
	ar rvs safari.a $(OBJS)