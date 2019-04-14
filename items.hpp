#ifndef _ITEMS_
#define _ITEMS_

#include <string>

struct Item {
	std::string name;
	int quantity;
};

struct ShippingItem {
	std::string name;
	int quantity, needed;
};

#endif
