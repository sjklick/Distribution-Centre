#ifndef __ORDER_PICKER__
#define __ORDER_PICKER__

#include <string>
#include "position.hpp"

namespace Picker {
	bool is_assigned (int pickerId);
	void assign_shipping_task (int pickerId, std::string item, int bin);
	void assign_receiving_task (int pickerId, std::string item, int bin);
	Position get_position (int pickerId);
	void update (int pickerId);
}

#endif
