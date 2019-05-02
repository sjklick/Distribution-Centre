CFLAGS = -std=c++11

all: warehouse

debug: CFLAGS += -ggdb
debug: warehouse

warehouse: main.o controller.o json-writer.o order-picker.o database-access.o state.o position.o
	g++ $(CFLAGS) -o warehouse.out main.o controller.o json-writer.o order-picker.o database-access.o state.o position.o `mysql_config --libs`

main.o: main.cpp
	g++ $(CFLAGS) -c main.cpp

controller.o: controller.hpp controller.cpp
	g++ $(CFLAGS) -c controller.cpp

json-writer.o: json-writer.hpp json-writer.cpp
	g++ $(CFLAGS) -c json-writer.cpp

order-picker.o: order-picker.hpp order-picker.cpp
	g++ $(CFLAGS) -c order-picker.cpp

database-access.o: database-access.hpp database-access.cpp
	g++ $(CFLAGS) `mysql_config --include` -c database-access.cpp

state.o: state.hpp state.cpp
	g++ $(CFLAGS) -c state.cpp

position.o: position.hpp position.cpp
	g++ $(CFLAGS) -c position.cpp

clean:
	rm -f *.o *.out *.json
