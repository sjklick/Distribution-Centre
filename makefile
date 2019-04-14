CFLAGS = -std=c++11

all: warehouse

debug: CFLAGS += -ggdb
debug: warehouse

warehouse: main.o json-writer.o database-access.o order-picker.o position.o
	g++ $(CFLAGS) -o warehouse.out main.o json-writer.o database-access.o order-picker.o position.o `mysql_config --libs`

main.o: main.cpp
	g++ $(CFLAGS) -c main.cpp

json-writer.o: json-writer.hpp json-writer.cpp
	g++ $(CFLAGS) -c json-writer.cpp

database-access.o: database-access.hpp database-access.cpp
	g++ $(CFLAGS) `mysql_config --cxxflags` -c database-access.cpp

order-picker.o: order-picker.hpp order-picker.cpp
	g++ $(CFLAGS) -c order-picker.cpp

position.o: position.hpp position.cpp
	g++ $(CFLAGS) -c position.cpp

clean:
	rm -f *.o *.out *.json
