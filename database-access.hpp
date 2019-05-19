#ifndef __DATABASE_ACCESS__
#define __DATABASE_ACCESS__

#include <vector>
#include <string>
#include "position.hpp"
#include "items.hpp"
#include "state.hpp"

namespace Database {
	// Stock bin related functions.
	std::vector<int> stock_get_id_list ();
	Position stock_get_position (int binId);
	int stock_get_item_count (int binId);
	std::vector<Item> stock_get_contents (int binId);
	std::vector<int> stock_find_bins_with_room ();
	int stock_find_first_item_location (std::string item);
	int stock_where_to_take_item (std::string item);	// IMPLEMENT
	int stock_where_to_place_item ();	// IMPLEMENT

	// Receiving bin related functions.
	std::string receiving_get_next_item_to_stock ();	// IMPLEMENT
	std::vector<std::string> receiving_get_items ();
	void receiving_replenish ();

	// Shipping bin related functions.
	void shipping_clear ();

	// Customer order related functions.
	int order_get_current ();
	std::string order_get_next_item_to_ship (int orderId);	// IMPLEMENT
	bool order_check_if_ready (int orderId);
	std::vector<Item> order_get_items (int orderId);
	void order_remove_items (int orderId);
	void order_remove (int orderId);

	// Picker related functions.
	std::vector<int> picker_get_id_list ();
	State picker_get_state (int pickerId);
	void picker_set_state (int pickerId, State state);
	Position picker_get_home (int pickerId);
	Position picker_get_current (int pickerId);
	Position picker_get_target (int pickerId);	// IMPLEMENT
	void picker_set_current (int pickerId, Position current);
	void picker_set_target (int pickerId, Position current);	// IMPLEMENT
	void picker_take_item_from_receiving (int pickerId, std::string itemName);
	void picker_take_item_from_stock (int pickerId, std::string itemName, int binId);
	void picker_take_item_from_receiving (int pickerId);	// IMPLEMENT
	void picker_take_item_from_stock (int pickerId);	// IMPLEMENT
	void picker_place_item_into_stock (int pickerId, std::string itemName, int binId);
	void picker_place_item_into_shipping (int pickerId, std::string itemName);
	void picker_place_item_into_stock (int pickerId);	// IMPLEMENT
	void picker_place_item_into_shipping (int pickerId);	// IMPLEMENT
	bool picker_check_if_assigned (int pickerId);	// IMPLEMENT
	bool picker_is_task_complete (int pickerId);	// IMPLEMENT
	bool picker_is_task_ship (int pickerId);	// IMPLEMENT
	bool picker_is_task_receive (int pickerId);	// IMPLEMENT
	bool picker_has_item (int pickerId);	// IMPLEMENT
	void picker_assign_shipping_task (int pickerId, std::string item, int binId);	// IMPLEMENT
	void picker_assign_receiving_task (int pickerId, std::string item, int binId);	// IMPLEMENT
	int picker_get_yield_count (int pickerId);
	void picker_increment_yield_count (int pickerId);
	void picker_reset_yield_count (int pickerId);
}

#endif
