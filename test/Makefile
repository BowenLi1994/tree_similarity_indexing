Incdir := $(shell find ../src -maxdepth 1 -type d)
IncdirAll := $(foreach p, $(Incdir), -I$(p))
CFLAGS += $(IncdirAll)

Debug_Level=-O3 -Wall 
CC=g++
CXXFLAGS=-c  $(Debug_Level) -std=c++17  -Wall

indexing:
	$(CC) $(CFLAGS) $(CXXFLAGS) indexing_test.cpp
	$(CC) -o indexing_test indexing_test.o 

indexing_join:
	$(CC) $(CFLAGS) $(CXXFLAGS) indexing_join.cpp
	$(CC) -o indexing_join indexing_join.o
	rm indexing_join.o 

feature_indexing:
	$(CC) $(CFLAGS) $(CXXFLAGS) feature_indexing.cpp
	$(CC) -o feature_indexing *.o 

resize:
	$(CC) $(CFLAGS) $(CXXFLAGS) resize.cpp
	$(CC) -o resize resize.o
	rm resize.o 

feature_join:
	$(CC) $(CFLAGS) $(CXXFLAGS) feature_join.cpp
	$(CC) -o feature_join  feature_join.o
	rm feature_join.o

tjoin_test:
	$(CC) $(CFLAGS) $(CXXFLAGS) tjoin_test.cpp
	$(CC) -o tjoin_test tjoin_test.o
	rm tjoin_test.o 
distance_indexing:
	$(CC) $(CFLAGS) $(CXXFLAGS) distance_indexing.cpp
	$(CC) -o distance_indexing distance_indexing.o
	rm distance_indexing.o
fjoin_ti:
	$(CC) $(CFLAGS) $(CXXFLAGS) fjoin_ti.cpp
	$(CC) -o fjoin_ti fjoin_ti.o
	rm fjoin_ti.o
fjoin_rn:
	$(CC) $(CFLAGS) $(CXXFLAGS) fjoin_rn.cpp
	$(CC) -o fjoin_rn fjoin_rn.o
	rm fjoin_rn.o


clean:
	rm -f ./*.o indexing_test feature_indexing resize feature_join indexing_join tjoin_test fjoin_ti fjoin_rn