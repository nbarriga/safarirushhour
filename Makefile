CXXFLAGS = -O3 -msse -msse2 -Wall -std=c++11 
CXX=PATH=/local/scratch/mburo/etc/bin:${PATH} LD_LIBRARY_PATH=/local/scratch/mburo/etc/lib:/local/scratch/mburo/etc/lib32:/local/scratch/mburo/etc/lib64:${LD_LIBRARY_PATH} g++
OBJS =	State.o Node.o Puzzle.o Safari.o  

LIBS =

all:	safari-bin 

safari-bin:	$(OBJS) 
	$(CXX)  -o safari-bin $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS) safari-bin $(TARGET)
