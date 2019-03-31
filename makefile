all: main.o order-picker.o position.o
	g++ -std=c++11 -o warehouse main.o order-picker.o position.o

main.o: main.cpp
	g++ -std=c++11 -c main.cpp

order-picker.o: order-picker.hpp order-picker.cpp
	g++ -std=c++11 -c order-picker.cpp

position.o: position.hpp position.cpp
	g++ -std=c++11 -c position.cpp

clean:
	rm -f *.o *.out
