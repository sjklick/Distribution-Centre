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

struct Item {
	std::string name;
	int quantity;
};

char DirectionToChar(Direction dir) {
    switch (dir) {
        case up:    return 'u';
        case down:  return 'd';
        case left:  return 'l';
        case right: return 'r';
        default:    return '?';
    }
}

Direction CharToDirection(char c) {
	switch (c) {
		case 'u':	return up;
		case 'd':	return down;
		case 'l':	return left;
		case 'r':	return right;
		default:	return invalid;
	}
}

std::string StateToString(State state) {
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

// Returns NULL if bin ID is not found or there was an error.
Position* getBinPosition(MYSQL* connection, int binId) {
	Position* bin;
	MYSQL_RES* result;
	MYSQL_ROW row;
	std::string query("SELECT * FROM stock_bins WHERE bin_id="+std::to_string(binId)+";");
	if (mysql_query(connection, query.c_str()) == 0) {
		result = mysql_use_result(connection);
		if (result) {
			bin = new Position;
			if(row = mysql_fetch_row(result)) {
				bin->row = std::stoi(row[1]);
				bin->column = std::stoi(row[2]);
				bin->facing = CharToDirection(row[3][0]);
				mysql_free_result(result);
				return bin;
			}
		}
	}
	return NULL;
}

// Returns -1 if bin ID is not found or there was an error.
int getBinItemCount(MYSQL* connection, int binId) {
	int nItems;
	MYSQL_RES* result;
	MYSQL_ROW row;
	std::string query("SELECT SUM(quantity) FROM stock_items WHERE bin_id="+std::to_string(binId)+";");
	if (mysql_query(connection, query.c_str()) == 0) {
		result = mysql_use_result(connection);
		if (result) {
			if(row = mysql_fetch_row(result)) {
				nItems = std::stoi(row[0]);
				mysql_free_result(result);
				return nItems;
			}
		}
	}
	return -1;
}

// Returns -1 if no orders or there was an error.
int getNextOrderId(MYSQL* connection) {
	int nextId;
	MYSQL_RES* result;
	MYSQL_ROW row;
	std::string query("SELECT * FROM orders LIMIT 1;");
	if (mysql_query(connection, query.c_str()) == 0) {
		result = mysql_use_result(connection);
		if (result) {
			if(row = mysql_fetch_row(result)) {
				nextId = std::stoi(row[0]);
				mysql_free_result(result);
				return nextId;
			}
		}
	}
	return -1;
}

std::vector<Item> getBinContents(MYSQL* connection, int binId) {
	std::vector<Item> items;
	Item temp;
	MYSQL_RES* result;
	MYSQL_ROW row;
	std::string query("SELECT * FROM stock_items WHERE bin_id="+std::to_string(binId)+";");
	if (mysql_query(connection, query.c_str()) == 0) {
		result = mysql_use_result(connection);
		if (result) {
			while (row = mysql_fetch_row(result)) {
				temp.name = std::string(row[1]);
				temp.quantity = std::stoi(row[2]);
				items.push_back(temp);
			}
			mysql_free_result(result);
		}
	}
	return items;
}

void writeStateJSON(int currentOrderId, int numPickers, OrderPicker* picker[], int numBins, Position* bin[], int nItems[]) {
	Position current;
	std::ofstream stateFile;
	stateFile.open("state.json", std::ios::out | std::ios::trunc);
	stateFile << "{";

	// order
	stateFile << "\"order\":";
	stateFile << currentOrderId;

	// pickers
	stateFile << ",\"picker\":[";
	for (int i=0; i<numPickers; i++) {
		current = picker[i]->getPosition();
		stateFile << "{\"position\":{";
		stateFile << "\"row\":";
		stateFile << current.row;
		stateFile << ",\"column\":";
		stateFile << current.column;
		stateFile << ",\"facing\":";
		stateFile << "\"" << DirectionToChar(current.facing) << "\"}";
		stateFile << ",\"state\":";
		stateFile << "\"" << StateToString(picker[i]->getState()) << "\"";
		stateFile << "}";
		if (i != (numPickers-1)) stateFile << ",";
	}
	stateFile << "]";

	// bins
	stateFile << ",\"bin\":[";
	for (int i=0; i<numBins; i++) {
		current = *bin[i];
		stateFile << "{\"position\":{";
		stateFile << "\"row\":";
		stateFile << current.row;
		stateFile << ",\"column\":";
		stateFile << current.column;
		stateFile << ",\"facing\":";
		stateFile << "\"" << DirectionToChar(current.facing) << "\"}";
		stateFile << ",\"nItems\":";
		stateFile << nItems[i];
		stateFile << "}";
		if (i != (numBins-1)) stateFile << ",";
	}
	stateFile << "]";

	stateFile << "}";
	stateFile.close();
}

// CHECK: Will this crash and burn if the bin is empty?
void writeBinJSON(int binId, std::vector<Item> binContents) {
	std::ofstream binFile;
	binFile.open("bin_"+std::to_string(binId)+".json", std::ios::out | std::ios::trunc);
	binFile << "{\"item\":[";

	for (std::vector<Item>::iterator it = binContents.begin(); it != binContents.end(); it++) {
		binFile << "{\"name\":";
		binFile << "\"" << (*it).name << "\",";
		binFile << "\"quantity\":";
		binFile << std::to_string((*it).quantity) << "}";
		if (binContents.end() - it != 1) binFile << ",";
	}

	binFile << "]}";
	binFile.close();
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

	// Make SQL server connection.
	MYSQL* connection;
	connection = mysql_init(NULL);
	if (connection != NULL) {
		std::cout << "Successful init!" << std::endl;
		if (!mysql_real_connect(connection, "localhost", username.c_str(), password.c_str(), "stock", 0, NULL, 0)) {
			std::cout << "Failed real init." << std::endl;
			std::cout << std::string(mysql_error(connection)) << std::endl;
		} else {
			std::cout << "Successful real init!" << std::endl;;
		}
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

	// Get bin positions from server.
	const int numBins = 22;
	Position* bin[numBins];
	int nItems[numBins];
	for (int i=0; i<numBins; i++) {
		bin[i] = getBinPosition(connection, i+1);
		nItems[i] = getBinItemCount(connection, i+1);
	}

	Position home[4];
	const int numPickers = 4;
	OrderPicker* picker[numPickers];

    home[0].row = 1;
    home[0].column = 1;
    home[0].facing = right;
	picker[0] = new OrderPicker(home[0]);
	picker[0]->processItem(*bin[0]);

	home[1].row = 2;
    home[1].column = 1;
    home[1].facing = right;
	picker[1] = new OrderPicker(home[1]);
	picker[1]->processItem(*bin[5]);

	home[2].row = 3;
    home[2].column = 1;
    home[2].facing = right;
	picker[2] = new OrderPicker(home[2]);
	picker[2]->processItem(*bin[10]);

	home[3].row = 4;
    home[3].column = 1;
    home[3].facing = right;
	picker[3] = new OrderPicker(home[3]);
	picker[3]->processItem(*bin[15]);

	// Get initial bin contents.
	for (int i=0; i<numBins; i++) {
		std::vector<Item> currentBin = getBinContents(connection, i+1);
		writeBinJSON(i+1, currentBin);
	}

    Position current;
	std::stringstream ss;
	int currentOrderId = -1;
    while (true) {

		writeStateJSON(currentOrderId, numPickers, picker, numBins, bin, nItems);

		// Check for a new order.
		if (currentOrderId == -1) currentOrderId = getNextOrderId(connection);

		// TEMPORARY
		// If all robots are idle, kill the process so that the server connection ends cleanly.
		bool allIdle = true;
		for (int i=0; i<numPickers; i++) {
			if (picker[i]->getState() != State::idle) allIdle = false;
		}
		if (allIdle) {
			/*for (int i=0; i<numPickers; i++) {
				picker[i]->processItem(bin[i]);
			}*/
			break;
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

	// Cleanly close SQL server connection.
	if (connection != NULL) mysql_close(connection);

	// Cleanly free memory.
	for (int i=0; i<numBins; i++) delete bin[i];
}
