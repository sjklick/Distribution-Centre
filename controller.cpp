#include "controller.hpp"
#include "json-writer.hpp"

Controller::Controller() {
	currentOrderId = -1;
	timeUntilRestock = 30;
}

bool Controller::init() {
	// Get picker start positions.
	Position home;
	for (int i=0; i<numPickers; i++) {
		home = db.getPickerHome(i+1);
		if ((home.row != -1) && (home.column != -1) && (home.facing != '?')) {
			picker[i] = new OrderPicker(i+1, home);
		} else return false;
	}

	// Get bin positions from server.
	for (int i=0; i<numBins; i++) {
		bin[i] = db.getBinPosition(i+1);
		if ((bin[i].row == -1) && (bin[i].column == -1) && (bin[i].facing == '?')) return false;
		nItems[i] = db.getBinItemCount(i+1);
		if (nItems[i] == -1) return false;
	}

	// Everything went okay.
	return true;
}

bool Controller::readState() {
	return true;
}

void Controller::updateState() {
	// Update time until new stock arrives.
	if (db.getReceivingItems().empty() && (timeUntilRestock == 0)) {
		db.placeNewStock(db.getLowInventory());
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
								picker[i]->processItem(binId, bin[binId-1], (*it).name);
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
										picker[i]->stockItem((*it), bin[(*it)-1], (*itemIt));
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
					currentBin = db.getBinContents(binId);
					nItems[binId-1] = db.getBinItemCount(binId);
					break;
				}
				binId = picker[i]->getStockBin();
				if (binId != -1) {
					// Remove item from receiving bin.
					itemName = picker[i]->getStockItemName();
					db.removeItemFromReceiving(itemName);
				}
				break;
			case State::place:
				binId = picker[i]->getTargetBinId();
				if (binId != -1) {
					db.putItemInShipping(picker[i]->getItemName());
					shippingItems = db.getShippingContents();
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
					nItems[binId-1] = db.getBinItemCount(binId);
				}
				break;
		}
	}

	// Create path map.
	char path_map[10][10];
	for (int row=0; row<10; row++) {
		for (int column=0; column<10; column++) {
			if ((row == 0) || (row == 9) || (column == 0) || (column == 9)) {
				path_map[row][column] = 'X';
			} else {
				path_map[row][column] = '.';
			}
		}
	}
	for (int i=0; i<numBins; i++) {
		path_map[bin[i].row][bin[i].column] = 'X';
	}
	// add picker positions to path map
	for (int i=0; i<numPickers; i++) {
		Position current = picker[i]->getPosition();
		path_map[current.row][current.column] = 'X';
	}

	// Move pickers to new positions.
	for (int i=0; i<numPickers; i++) {
		picker[i]->update(path_map);
	}
}

bool Controller::writeState() {
	writeStateJSON(currentOrderId, numPickers, picker, numBins, bin, nItems);
	writeReceivingJSON(db.getReceivingItems());
	writeShippingJSON(shippingItems);
	for (int i=0; i<numBins; i++) {
		writeBinJSON(i+1, db.getBinContents(i+1));
	}
	return true;
}

Controller::~Controller() {
}
