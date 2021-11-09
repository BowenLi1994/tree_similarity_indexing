Incdir := $(shell find ../src -maxdepth 1 -type d)
IncdirAll := $(foreach p, $(Incdir), -I$(p))
CFLAGS += $(IncdirAll)

Debug_Level=-O3 -Wall 
CC=g++
CXXFLAGS=-c  $(Debug_Level) -std=c++17  -Wall

clean_data:
	$(CC) $(CFLAGS) $(CXXFLAGS) clean_data.cpp
	$(CC) -o clean_data clean_data.o
	rm clean_data.o
analyzer:
	$(CC) $(CFLAGS) $(CXXFLAGS) analyzer.cpp
	$(CC) -o analyzer analyzer.o
	rm analyzer.o
relabel:
	$(CC) $(CFLAGS) $(CXXFLAGS) relabel.cpp
	$(CC) -o relabel relabel.o
	rm relabel.o	
	



clean:
	rm -f ./*.o clean_data relabel analyzer