#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <sstream>
#include <chrono>
#include <thread>
#include "position.hpp"
#include "order-picker.hpp"

char DirectionToChar (Direction dir) {
    switch (dir) {
        case up:    return 'u';
        case down:  return 'd';
        case left:  return 'l';
        case right: return 'r';
        default:    return '?';
    }
}

std::string StateToString (State state) {
	switch (state) {
		case State::idle:		return "idle";
		case State::yield:		return "yield";
		case State::home:		return "home";
		case State::retrieve:	return "retrieve";
		case State::stock:		return "stock";
		case State::receive:	return "receive";
		case State::ship:		return "ship";
		case State::extend:		return "extend";
		case State::retract:	return "retract";
		case State::pick:		return "pick";
		case State::place:		return "place";
		default:				return "?";
	}
}

int main() {
    // initial warehouse map
    // 'X' => wall, 'B' => bin, 'S' => shipping, '.' => empty
    char init_map[10][10] = {{'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'},
                             {'X', '.', '.', '.', '.', '.', '.', '.', '.', 'X'},
                             {'X', '.', '.', 'B', 'B', '.', 'B', 'B', '.', 'X'},
                             {'X', '.', '.', 'B', 'B', '.', 'B', 'B', '.', 'X'},
                             {'X', '.', '.', 'B', 'B', '.', 'B', 'B', '.', 'X'},
                             {'X', '.', '.', '.', '.', '.', 'B', 'B', '.', 'X'},
                             {'X', '.', '.', 'B', 'B', '.', 'B', 'B', '.', 'X'},
                             {'X', '.', '.', 'B', 'B', '.', 'B', 'B', '.', 'X'},
                             {'X', '.', '.', '.', '.', '.', '.', '.', '.', 'X'},
                             {'X', 'S', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'}};
    char display_map[10][10];
	char path_map[10][10];

	Position home[4];
	Position bin[4];
	const int numPickers = 4;
	OrderPicker* picker[numPickers];

    home[0].row = 1;
    home[0].column = 1;
    home[0].facing = right;
    bin[0].row = 7;
    bin[0].column = 4;
    bin[0].facing = right;
	picker[0] = new OrderPicker(home[0]);
	picker[0]->processItem(bin[0]);

	home[1].row = 2;
    home[1].column = 1;
    home[1].facing = right;
    bin[1].row = 7;
    bin[1].column = 6;
    bin[1].facing = down;
	picker[1] = new OrderPicker(home[1]);
	picker[1]->processItem(bin[1]);

	home[2].row = 3;
    home[2].column = 1;
    home[2].facing = right;
    bin[2].row = 6;
    bin[2].column = 6;
    bin[2].facing = left;
	picker[2] = new OrderPicker(home[2]);
	picker[2]->processItem(bin[2]);

	home[3].row = 4;
    home[3].column = 1;
    home[3].facing = right;
    bin[3].row = 4;
    bin[3].column = 7;
    bin[3].facing = right;
	picker[3] = new OrderPicker(home[3]);
	picker[3]->processItem(bin[3]);

    Position current;
	std::stringstream ss;
    while (true) {

		// TEMPORARY
		// If all robots are idle, restart the process.
		bool allIdle = true;
		for (int i=0; i<numPickers; i++) {
			if (picker[i]->getState() != State::idle) allIdle = false;
		}
		if (allIdle) {
			for (int i=0; i<numPickers; i++) {
				picker[i]->processItem(bin[i]);
			}
		}

        // map copy for display
        memcpy(display_map, init_map, 100);
        // add current picker positions to display map
		for (int i=0; i<numPickers; i++) {
        	current = picker[i]->getPosition();
			ss.str("");
			ss << i;
        	display_map[current.row][current.column] = ss.str()[0];
		}
        // print map
        system("clear");
        for (int row=0; row<10; row++) {
            for (int column=0; column<10; column++) {
                std::cout << display_map[row][column];
            }
            std::cout << std::endl;
        }
		// print picker info
		for (int i=0; i<numPickers; i++) {
			std::cout << "Picker " << i << ": state=" << StateToString(picker[i]->getState());
			std::cout << " facing=" << DirectionToChar(picker[i]->getPosition().facing);
			if (picker[i]->hasItem()) std::cout << " item=yes" << std::endl;
			else std::cout << " item=no" << std::endl;
		}
        // Sleep for 1 second.
        std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::seconds(1));
		// map copy for path calculations
        memcpy(path_map, init_map, 100);
		// add picker positions to path map
		for (int i=0; i<numPickers; i++) {
			current = picker[i]->getPosition();
			path_map[current.row][current.column] = 'X';
		}
		for (int i=0; i<numPickers; i++) {
        	picker[i]->update(path_map);
		}
    }
}
