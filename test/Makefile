Incdir := $(shell find ../src -maxdepth 1 -type d)
IncdirAll := $(foreach p, $(Incdir), -I$(p))
CFLAGS += $(IncdirAll)

Debug_Level=-O3 -Wall 
CC=g++
CXXFLAGS=-c  $(Debug_Level) -std=c++17  -Wall

indexing:
	$(CC) $(CFLAGS) $(CXXFLAGS) indexing_test.cpp
	$(CC) -o indexing_test *.o 

feature_indexing:
	$(CC) $(CFLAGS) $(CXXFLAGS) feature_indexing.cpp
	$(CC) -o feature_indexing *.o 

clean:
	rm -f ./*.o indexing_test feature_indexing