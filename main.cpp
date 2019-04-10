#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <sstream>
#include <chrono>
#include <thread>
#include <fstream>
#include "position.hpp"
#include "order-picker.hpp"
#include "database-access.hpp"

struct Item;

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

/*int whichBinHasItem(MYSQL* connection, Item item) {
}*/

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
		stateFile << ",\"hasItem\":";
		if (picker[i]->hasItem()) stateFile << "true";
		else stateFile << "false";
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
	Database db;
	db.connect();

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
		bin[i] = db.getBinPosition(i+1);
		nItems[i] = db.getBinItemCount(i+1);
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
		std::vector<Item> currentBin = db.getBinContents(i+1);
		writeBinJSON(i+1, currentBin);
	}

    Position current;
	std::stringstream ss;
	int currentOrderId = -1;
	std::vector<Item> orderItems;
    while (true) {

		writeStateJSON(currentOrderId, numPickers, picker, numBins, bin, nItems);

		// Check for a new order.
		if (currentOrderId == -1) {
			// Update order number.
			currentOrderId = db.getNextOrderId();
			// Get list of items required.
			orderItems = db.getOrderItems(currentOrderId);
		}

		// Check for any idle pickers.
		/*for (int i=0; i<numPickers; i++) {
			if (picker[i]->getState() == State::idle) {
				// If there are any outstanding order items, process them.
				if (!orderItems.empty()) {
					int binId;
					binId = whichBinHasItem(connection, orderItems.back());
					picker[i]->processItem(*bin[binId-1]);
					// Decrement quantity, remove from list if fulfilled.
					orderItems.back().quantity--;
					if (orderItems.back() == 0) orderItems.pop_back();
				}
			}
		}*/

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

	db.disconnect();

	// Cleanly free memory.
	for (int i=0; i<numBins; i++) delete bin[i];
}
