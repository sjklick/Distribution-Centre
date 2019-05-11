#ifndef __DATABASE_ACCESS__
#define __DATABASE_ACCESS__

#include <vector>
#include <string>
#include <mysql/mysql.h>
#include "position.hpp"
#include "items.hpp"
#include "state.hpp"

namespace Database {
	void emptyShippingBin();
	std::vector<std::string> getLowInventory();
	std::vector<std::string> getReceivingItems();
	void placeNewStock(std::vector<std::string> itemNames);

	// Stock bin related functions.
	Position stock_get_position (int binId);
	int stock_get_item_count (int binId);
	std::vector<Item> stock_get_contents (int binId);
	std::vector<int> stock_find_bins_with_room ();
	int stock_find_first_item_location (std::string item);

	// Customer order related functions.
	int order_get_current ();
	bool order_check_if_ready (int orderId);
	std::vector<Item> order_get_items (int orderId);
	void order_remove_items (int orderId);
	void order_remove (int orderId);

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
