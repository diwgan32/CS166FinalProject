CXX = g++
CXXFLAGS = -std=c++11 -Wall -g

main: main.o TarjanSoftHeap.o
	$(CXX) $(CXXFLAGS) -o main main.o TarjanSoftHeap.o

main.o: main.cpp TarjanSoftHeap.h
	$(CXX) $(CXXFLAGS) -c main.cpp

TarjanSoftHeap.o: TarjanSoftHeap.h

clean:
	rm -rf *.o main
