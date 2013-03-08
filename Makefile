#CXXFLAGS = -g -Wall -std=c++11 
CXXFLAGS = -O3 -msse -msse2 -Wall -std=c++11 
#CXXFLAGS = -O3 -g -Wall -std=c++11 
#CXXFLAGS = -O3 -ftree-vectorizer-verbose=2 -msse -msse2 -Wall -std=c++11 
CXX=PATH=/local/scratch/mburo/etc/bin:${PATH} LD_LIBRARY_PATH=/local/scratch/mburo/etc/lib:/local/scratch/mburo/etc/lib32:/local/scratch/mburo/etc/lib64:${LD_LIBRARY_PATH} g++
OBJS = Common.o State.o TranspositionTable.o Puzzle.o 

LIBS = -L/local/scratch/mburo/etc/lib -lboost_program_options

all:	safari-bin verify

safari-bin:	$(OBJS) Safari.o
	$(CXX)   -o safari-bin $(OBJS) Safari.o $(LIBS)

verify:	$(OBJS) Verify.o
	$(CXX)  -o verify $(OBJS) Verify.o $(LIBS)
clean:
	rm -f $(OBJS) safari-bin $(TARGET)
