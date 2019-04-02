#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <sstream>
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

    Position home, bin;
	const int numPickers = 4;
	OrderPicker* picker[numPickers];

    home.row = 1;
    home.column = 1;
    home.facing = right;
    bin.row = 7;
    bin.column = 4;
    bin.facing = right;
	picker[0] = new OrderPicker(home);
	picker[0]->processItem(bin);

	home.row = 2;
    home.column = 1;
    home.facing = right;
    bin.row = 7;
    bin.column = 6;
    bin.facing = down;
	picker[1] = new OrderPicker(home);
	picker[1]->processItem(bin);

	home.row = 3;
    home.column = 1;
    home.facing = right;
    bin.row = 6;
    bin.column = 6;
    bin.facing = left;
	picker[2] = new OrderPicker(home);
	picker[2]->processItem(bin);

	home.row = 4;
    home.column = 1;
    home.facing = right;
    bin.row = 4;
    bin.column = 7;
    bin.facing = right;
	picker[3] = new OrderPicker(home);
	picker[3]->processItem(bin);

	char c = ' ';
    Position current;
	std::stringstream ss;
    while (c != 'q') {
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
        // wait for user to enter char
        std::cin.get(c);
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
