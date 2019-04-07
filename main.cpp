#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <sstream>
#include <chrono>
#include <thread>
#include <fstream>
#include <mysql.h>
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
	// Load credentials.
	// Save username and password on separate lines of "credentials.txt".
	std::ifstream credFile;
	std::string username, password;
	credFile.open("credentials.txt", std::ios::in);
	credFile >> username;
	credFile >> password;
	credFile.close();

	// MariaDB test.
	MYSQL* connection;
	MYSQL_RES* result;
	//MYSQL_FIELD* field;
	unsigned int numFields;
	MYSQL_ROW row;
	connection = mysql_init(NULL);
	if (connection != NULL) {
		std::cout << "Successful init!" << std::endl;
		if (!mysql_real_connect(connection, "localhost", username.c_str(), password.c_str(), "stock", 0, NULL, 0)) {
			std::cout << "Failed real init." << std::endl;
			std::cout << std::string(mysql_error(connection)) << std::endl;
		} else {
			std::cout << "Successful real init!" << std::endl;
			if (mysql_query(connection, "SHOW TABLES;") == 0) {
				std::cout << "Successful query!" << std::endl;
				result = mysql_use_result(connection);
				if (!result) std::cout << "No result set." << std::endl;
				else {
					std::cout << "Have results!" << std::endl;
					numFields = mysql_num_fields(result);
					std::cout << "There are " << numFields << " field(s)." << std::endl;
					for (int i=0; i<numFields; i++) {
						//field = mysql_fetch_field(result);
						while(row = mysql_fetch_row(result)) std::cout << row[i] << std::endl;
					}
				}
			} else std::cout << "Failed query." << std::endl;
		}
		std::cout << "Closing connection." << std::endl;
		mysql_close(connection);
	}
	else std::cout << "Failed init." << std::endl;

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

		// Write a JSON file for picker positions.
		// [{"row":1,"column":1,"facing":"r"},{"row":2,"column":1,"facing":"r"},{"row":3,"column":1,"facing":"u"},{"row":4,"column":1,"facing":"r"}]
		std::ofstream pickerFile;
		pickerFile.open("pickers.json", std::ios::out | std::ios::trunc);
		pickerFile << "[";
		for (int i=0; i<numPickers; i++) {
			current = picker[i]->getPosition();
			pickerFile << "{";
			pickerFile << "\"row\":";
			pickerFile << current.row;
			pickerFile << ",\"column\":";
			pickerFile << current.column;
			pickerFile << ",\"facing\":";
			pickerFile << "\"" << DirectionToChar(current.facing) << "\"";
			pickerFile << ",\"state\":";
			pickerFile << "\"" << StateToString(picker[i]->getState()) << "\"";
			pickerFile << "}";
			if (i != (numPickers-1)) pickerFile << ",";
		}
		pickerFile << "]";
		pickerFile.close();

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
