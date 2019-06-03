CXX = g++
CXXFLAGS = -std=c++11 -Wall -g

main: main.o LCRSSoftHeap.o
	$(CXX) $(CXXFLAGS) -o main main.o LCRSSoftHeap.o

main.o: main.cpp LCRSSoftHeap.h
	$(CXX) $(CXXFLAGS) -c main.cpp

SoftHeap.o: LCRSSoftHeap.h

clean:
	rm -rf *.o main
