#ifndef __ORDER_PICKER__
#define __ORDER_PICKER__

#include <vector>
#include <string>
#include "position.hpp"

/*
Order-picker states:
idle ------> at home, doing nothing
yield -----> stationary, can't reach target currently
home ------> heading back to home
retrieve --> getting an item from inventory bin
stock -----> going to place item in an inventory bin
receive ---> getting an item from a bin in receiving
ship ------> going to place an item in the current order's bin
extend ----> extend arms to bin
retract ---> bring arms back to rest position
pick ------> take the item from the bin
place -----> place the item in the bin
*/
enum class State { idle, yield, home, retrieve, stock, receive, ship, extend, retract, pick, place, extricate };

std::string StateToString(State state);

class OrderPicker {
private:
	Position home, current, target, extricate;
	std::vector<Position> path;
	State state;
	bool item, delivered, holdingStockItem, stockDelivered, stock;
	std::string itemName, stockItemName;
	int binId, stockBinId;
	int yieldCount;
public:
	OrderPicker(Position home);
	Position getPosition();
	std::vector<Position> getPath();
	State getState();
	bool hasItem();
	int getTargetBinId();
	std::string getItemName();
	bool processItem(int binId, Position bin, std::string itemName);
	void stockItem(int stockBinId, Position bin, std::string stockItemName);
	int getStockBin();
	std::string getStockItemName();
	void update(char map[10][10]);
	~OrderPicker();
};

#endif
