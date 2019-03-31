#ifndef __ORDER_PICKER__
#define __ORDER_PICKER__

#include <vector>
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
*/
enum class State { idle, yield, home, retrieve, stock, receive, ship };

class OrderPicker {
private:
	Position home, current, target;
	std::vector<Position> path;
	State state;
	bool hasItem;
public:
	OrderPicker(Position home);
	Position getPosition();
	std::vector<Position> getPath();
	State getState();
	bool processItem(Position bin);
	void update(char map[10][10]);
	~OrderPicker();
};

#endif
