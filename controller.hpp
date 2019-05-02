#ifndef _CONTROLLER_
#define _CONTROLLER_

#include "order-picker.hpp"
#include "position.hpp"
#include "items.hpp"
#include "database-access.hpp"

class Controller {
private:
	Database db;
	static const int numPickers = 4;
	OrderPicker* picker[numPickers];
	static const int numBins = 22;
	Position bin[numBins];
	int nItems[numBins];
	std::vector<ShippingItem> shippingItems;
	int currentOrderId;
	int timeUntilRestock;
	std::vector<Item> orderItems;
public:
	Controller();
	bool init();
	bool readState();
	void updateState();
	bool writeState();
	~Controller();
};

#endif
