#ifndef _JSON_WRITER_
#define _JSON_WRITER_

#include <string>
#include <vector>
#include "position.hpp"
#include "order-picker.hpp"
#include "items.hpp"

void writeStateJSON(int currentOrderId, int numPickers, OrderPicker* picker[], int numBins, Position* bin[], int nItems[]);
void writeBinJSON(int binId, std::vector<Item> binContents);
void writeShippingJSON(std::vector<ShippingItem> shippingContents);
void writeReceivingJSON(std::vector<std::string> receivingContents);

#endif
