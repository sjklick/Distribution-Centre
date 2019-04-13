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
struct ShippingItem;

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

void writeShippingJSON(std::vector<ShippingItem> shippingContents) {
	std::ofstream shippingFile;
	shippingFile.open("shipping.json", std::ios::out | std::ios::trunc);
	shippingFile << "{\"shippingItem\":[";

	for (std::vector<ShippingItem>::iterator it = shippingContents.begin(); it != shippingContents.end(); it++) {
		shippingFile << "{\"name\":";
		shippingFile << "\"" << (*it).name << "\",";
		shippingFile << "\"quantity\":";
		shippingFile << std::to_string((*it).quantity) << ",";
		shippingFile << "\"needed\":";
		shippingFile << std::to_string((*it).needed) << "}";
		if (shippingContents.end() - it != 1) shippingFile << ",";
	}

	shippingFile << "]}";
	shippingFile.close();
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

	home[1].row = 2;
    home[1].column = 1;
    home[1].facing = right;
	picker[1] = new OrderPicker(home[1]);

	home[2].row = 3;
    home[2].column = 1;
    home[2].facing = right;
	picker[2] = new OrderPicker(home[2]);

	home[3].row = 4;
    home[3].column = 1;
    home[3].facing = right;
	picker[3] = new OrderPicker(home[3]);

	// Get initial bin contents.
	for (int i=0; i<numBins; i++) {
		std::vector<Item> currentBin = db.getBinContents(i+1);
		writeBinJSON(i+1, currentBin);
	}

	// Setup initial (empty) shipping contents.
	std::vector<ShippingItem> shippingItems;
	writeShippingJSON(shippingItems);

    Position current;
	std::stringstream ss;
	int currentOrderId = -1;
	std::vector<Item> orderItems;
	int pickerAssignment[numBins];
    while (true) {

		writeStateJSON(currentOrderId, numPickers, picker, numBins, bin, nItems);

		// Check for a new order.
		if (currentOrderId == -1) {
			// Update order number.
			currentOrderId = db.getNextOrderId();
			// Get the manifest of order items.
			orderItems = db.getOrderItems(currentOrderId);
			// Initially, no bins are assigned to pickers.
			for (int i=0; i<numBins; i++) pickerAssignment[i] = -1;
			// Clear shipping bin.
			db.emptyShippingBin();
			// Setup shipping bin.
			db.prepareShippingBin(currentOrderId);
			// Remove items from order items (now tracked in shipping bin).
			db.removeOrderItems(currentOrderId);
			// Update shipping JSON with empty shipping bin;
			shippingItems.clear();
			writeShippingJSON(shippingItems);
		}

		// Check for any idle pickers, or if a bin can be unassigned.
		for (int i=0; i<numPickers; i++) {
			int binId;
			std::string itemName;
			std::vector<Item> currentBin;
			switch (picker[i]->getState()) {
				case State::idle:
					// If there are any outstanding order items, process them.
					if (currentOrderId != -1) {
						for (std::vector<Item>::iterator it=orderItems.begin(); it != orderItems.end(); it++) {
							if ((*it).quantity != 0) {
								int binId;
								binId = db.whichBinHasItem((*it).name);
								if ((binId != -1) && (pickerAssignment[binId-1] == -1)) {
									pickerAssignment[binId-1] = i;
									picker[i]->processItem(binId, *bin[binId-1], (*it).name);
									(*it).quantity--;
									break;
								}
							}
						}
					}
					break;
				case State::pick:
					for (int b=0; b<numBins; b++) {
						if (pickerAssignment[b] == i) {
							pickerAssignment[b] = -1;
							break;
						}
					}
					// Remove item from stock bin.
					binId = picker[i]->getTargetBinId();
					itemName = picker[i]->getItemName();
					db.removeItemFromStockBin(binId, itemName);
					// Update bin JSON.
					currentBin = db.getBinContents(binId);
					writeBinJSON(binId, currentBin);
					nItems[binId-1] = db.getBinItemCount(binId);
					break;
				case State::place:
					db.putItemInShipping(picker[i]->getItemName());
					shippingItems = db.getShippingContents();
					writeShippingJSON(shippingItems);
					if (db.orderFulfilled(currentOrderId)) {
						db.removeOrder(currentOrderId);
						currentOrderId = -1;
					}
					break;
			}
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
