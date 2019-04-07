CFLAGS = -std=c++11

all: warehouse

debug: CFLAGS += -ggdb
debug: warehouse

warehouse: main.o order-picker.o position.o
	g++ $(CFLAGS) -o warehouse.out main.o order-picker.o position.o `mysql_config --libs`

main.o: main.cpp
	g++ $(CFLAGS) `mysql_config --cxxflags` -c main.cpp

order-picker.o: order-picker.hpp order-picker.cpp
	g++ $(CFLAGS) -c order-picker.cpp

position.o: position.hpp position.cpp
	g++ $(CFLAGS) -c position.cpp

clean:
	rm -f *.o *.out
