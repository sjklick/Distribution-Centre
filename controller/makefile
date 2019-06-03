CFLAGS = -std=c++11

all: controller

debug: CFLAGS += -ggdb
debug: controller

controller: main.o order-picker.o floor-map.o database-access.o state.o position.o
	g++ $(CFLAGS) -o controller.out main.o order-picker.o floor-map.o database-access.o state.o position.o `mysql_config --libs`

main.o: main.cpp
	g++ $(CFLAGS) -c main.cpp

order-picker.o: order-picker.hpp order-picker.cpp
	g++ $(CFLAGS) -c order-picker.cpp

floor-map.o: floor-map.hpp floor-map.cpp
	g++ $(CFLAGS) -c floor-map.cpp

database-access.o: database-access.hpp database-access.cpp
	g++ $(CFLAGS) `mysql_config --include` -c database-access.cpp

state.o: state.hpp state.cpp
	g++ $(CFLAGS) -c state.cpp

position.o: position.hpp position.cpp
	g++ $(CFLAGS) -c position.cpp

clean:
	rm -f *.o *.out log.txt
