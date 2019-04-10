#ifndef __DATABASE_ACCESS__
#define __DATABASE_ACCESS__

#include <vector>
#include "position.hpp"

struct Item {
	std::string name;
	int quantity;
};

class Database {
	public:
	// Returns True if connection established.
	bool connect();

	// Cleanly closes database connection.
	void disconnect();

	// Returns NULL if bin ID is not found or there was an error.
	Position* getBinPosition(int binId);

	// Returns -1 if bin ID is not found or there was an error.
	int getBinItemCount(int binId);

	// Returns -1 if no orders or there was an error.
	int getNextOrderId();

	std::vector<Item> getBinContents(int binId);

	std::vector<Item> getOrderItems(int orderId);
};

#endif
