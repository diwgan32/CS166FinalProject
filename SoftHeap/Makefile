CXX = g++
CXXFLAGS = -std=c++11 -Wall -g

main: main.o SoftHeap.o
	$(CXX) $(CXXFLAGS) -o main main.o SoftHeap.o

main.o: main.cpp SoftHeap.h
	$(CXX) $(CXXFLAGS) -c main.cpp

SoftHeap.o: SoftHeap.h

clean:
	rm -rf *.o main
