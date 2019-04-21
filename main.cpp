#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <sstream>
#include <chrono>
#include <thread>
#include "position.hpp"
#include "order-picker.hpp"
#include "database-access.hpp"
#include "json-writer.hpp"
#include "items.hpp"

int main() {
	Database db;
	if (!db.connect()) return 1;

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

	// Get initial receiving contents.
	writeReceivingJSON(db.getReceivingItems());
	int timeUntilRestock = 30;

	// Setup initial (empty) shipping contents.
	std::vector<ShippingItem> shippingItems;
	writeShippingJSON(shippingItems);

	Position current;
	std::stringstream ss;
	int currentOrderId = -1;
	std::vector<Item> orderItems;
	while (true) {

		writeStateJSON(currentOrderId, numPickers, picker, numBins, bin, nItems);

		// Update time until new stock arrives.
		if (db.getReceivingItems().empty() && (timeUntilRestock == 0)) {
			db.placeNewStock(db.getLowInventory());
			writeReceivingJSON(db.getReceivingItems());
			timeUntilRestock = 30;
		} else if (db.getReceivingItems().empty()){
			timeUntilRestock--;
		}

		// Check for a new order.
		if (currentOrderId == -1) {
			// Update order number.
			currentOrderId = db.getNextOrderId();
			// Get the manifest of order items.
			orderItems = db.getOrderItems(currentOrderId);
			// Clear shipping bin.
			db.emptyShippingBin();
			// Setup shipping bin.
			db.prepareShippingBin(currentOrderId);
			// Remove items from order items (now tracked in shipping bin).
			db.removeOrderItems(currentOrderId);
			// Update shipping JSON with empty shipping bin;
			shippingItems = db.getShippingContents();
			writeShippingJSON(shippingItems);
		}

		// Check for any idle pickers, or if a bin can be unassigned.
		for (int i=0; i<numPickers; i++) {
			int binId;
			std::string itemName;
			std::vector<Item> currentBin;
			bool moreItems;
			switch (picker[i]->getState()) {
				case State::idle:
					moreItems = false;
					if (currentOrderId != -1) {
						// If there are any outstanding order items, process them.
						for (std::vector<Item>::iterator it=orderItems.begin(); it != orderItems.end(); it++) {
							if ((*it).quantity != 0) {
								int binId = db.whichBinHasItem((*it).name);
								bool assigned = false;
								for (int j=0; j<numPickers; j++) {
									if (picker[j]->getTargetBinId() == binId) {
										assigned = true;
										break;
									}
								}
								if ((binId != -1) && (!assigned)) {
									picker[i]->processItem(binId, *bin[binId-1], (*it).name);
									(*it).quantity--;
									moreItems = true;
									break;
								}
							}
						}
					}
					// If there are no more order items, check if there is anything in receiving.
					if (!moreItems) {
						if (!db.getReceivingItems().empty()) {
							std::vector<int> binsWithRoom = db.whichBinsHaveRoom();
							for (std::vector<int>::iterator it = binsWithRoom.begin(); it != binsWithRoom.end(); it++) {
								bool assigned = false;
								for (int j=0; j<numPickers; j++) {
									if (picker[j]->getStockBin() == (*it)) {
										assigned = true;
										break;
									}
								}
								if (!assigned) {
									std::vector<std::string> stockItems = db.getReceivingItems();
									for (std::vector<std::string>::iterator itemIt = stockItems.begin(); itemIt != stockItems.end(); itemIt++) {
										assigned = false;
										for (int j=0; j<numPickers; j++) {
											if (picker[j]->getStockItemName() == (*itemIt)) {
												assigned = true;
												break;
											}
										}
										if (!assigned) {
											picker[i]->stockItem((*it), *bin[(*it)-1], (*itemIt));
											break;
										}
									}
								}
							}
						}
					}
					break;
				case State::pick:
					binId = picker[i]->getTargetBinId();
					if (binId != -1) {
						// Remove item from stock bin.
						itemName = picker[i]->getItemName();
						db.removeItemFromStockBin(binId, itemName);
						// Update bin JSON.
						currentBin = db.getBinContents(binId);
						writeBinJSON(binId, currentBin);
						nItems[binId-1] = db.getBinItemCount(binId);
						break;
					}
					binId = picker[i]->getStockBin();
					if (binId != -1) {
						// Remove item from receiving bin.
						itemName = picker[i]->getStockItemName();
						db.removeItemFromReceiving(itemName);
						// Update receiving JSON.
						writeReceivingJSON(db.getReceivingItems());
					}
					break;
				case State::place:
					binId = picker[i]->getTargetBinId();
					if (binId != -1) {
						db.putItemInShipping(picker[i]->getItemName());
						shippingItems = db.getShippingContents();
						writeShippingJSON(shippingItems);
						if (db.orderFulfilled(currentOrderId)) {
							db.removeOrder(currentOrderId);
							currentOrderId = -1;
						}
						break;
					}
					binId = picker[i]->getStockBin();
					if (binId != -1) {
						db.placeItemIntoStockBin(binId, picker[i]->getStockItemName());
						currentBin = db.getBinContents(binId);
						writeBinJSON(binId, currentBin);
						nItems[binId-1] = db.getBinItemCount(binId);
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
