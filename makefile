CFLAGS = -std=c++11

all: warehouse

debug: CFLAGS += -ggdb
debug: warehouse

warehouse: main.o database-access.o order-picker.o position.o
	g++ $(CFLAGS) -o warehouse.out main.o database-access.o order-picker.o position.o `mysql_config --libs`

main.o: main.cpp
	g++ $(CFLAGS) -c main.cpp

database-access.o: database-access.hpp database-access.cpp
	g++ $(CFLAGS) `mysql_config --cxxflags` -c database-access.cpp

order-picker.o: order-picker.hpp order-picker.cpp
	g++ $(CFLAGS) -c order-picker.cpp

position.o: position.hpp position.cpp
	g++ $(CFLAGS) -c position.cpp

clean:
	rm -f *.o *.out *.json
