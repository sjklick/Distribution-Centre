#ifndef __DATABASE_ACCESS__
#define __DATABASE_ACCESS__

#include <vector>
#include <string>
#include <mysql/mysql.h>
#include "position.hpp"
#include "items.hpp"
#include "state.hpp"

namespace Database {
	Position getBinPosition(int binId);
	// Returns -1 if bin ID is not found or there was an error.
	int getBinItemCount(int binId);
	// Returns -1 if no orders or there was an error.
	int getNextOrderId();
	std::vector<Item> getBinContents(int binId);
	std::vector<Item> getOrderItems(int orderId);
	// Returns 1st bin found containing the item, or -1 otherwise.
	int whichBinHasItem(std::string item);
	// Updates the contents of the shipping bin to reflect what
	// is needed for the current order.
	void prepareShippingBin(int orderId);
	void emptyShippingBin();
	std::vector<ShippingItem> getShippingContents();
	bool orderFulfilled(int orderId);
	void removeOrder(int orderId);
	void removeOrderItems(int orderId);
	std::vector<std::string> getLowInventory();
	std::vector<std::string> getReceivingItems();
	void placeNewStock(std::vector<std::string> itemNames);
	std::vector<int> whichBinsHaveRoom();

	// Picker related functions.
	State picker_get_state (int pickerId);
	void picker_set_state (int pickerId, State state);
	Position picker_get_home (int pickerId);
	Position picker_get_current (int pickerId);
	void picker_set_current (int pickerId, Position current);
	void picker_take_item_from_receiving (int pickerId, std::string itemName);
	void picker_take_item_from_stock (int pickerId, std::string itemName, int binId);
	void picker_place_item_into_stock (int pickerId, std::string itemName, int binId);
	void picker_place_item_into_shipping (int pickerId, std::string itemName);
}

#endif
