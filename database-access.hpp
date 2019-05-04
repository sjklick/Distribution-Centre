#ifndef __DATABASE_ACCESS__
#define __DATABASE_ACCESS__

#include <vector>
#include <string>
#include <mysql/mysql.h>
#include "position.hpp"
#include "items.hpp"
#include "state.hpp"

class Database {
private:
	MYSQL* connection;

	// Returns True if connection established.
	bool connect();

	// Cleanly closes database connection.
	void disconnect();
public:
	Position getPickerHome(int pickerId);

	Position getPickerCurrent(int pickerId);

	void setPickerCurrent(int pickerId, Position current);

	State getPickerState(int pickerId);

	void setPickerState(int pickerId, State state);

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

	void putItemInShipping(std::string itemName);

	std::vector<ShippingItem> getShippingContents();

	bool orderFulfilled(int orderId);

	void removeOrder(int orderId);

	void removeItemFromStockBin(int binId, std::string itemName);

	void placeItemIntoStockBin(int binId, std::string itemName);

	// MAY NO LONGER BE NEEDED.
	void removeItemFromOrderItems(int orderId, std::string itemName);

	void removeOrderItems(int orderId);

	std::vector<std::string> getLowInventory();

	std::vector<std::string> getReceivingItems();

	void placeNewStock(std::vector<std::string> itemNames);

	std::vector<int> whichBinsHaveRoom();

	void removeItemFromReceiving (std::string itemName);
};

#endif
