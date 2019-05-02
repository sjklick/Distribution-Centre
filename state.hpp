#ifndef _STATE_
#define _STATE_

#include <string>

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
extricate -> Find a new temporary place to move to
invalid ---> something went wrong
*/
enum class State { idle, yield, home, retrieve, stock, receive, ship, extend, retract, pick, place, extricate, invalid };

std::string StateToString(State state);
State StringToState(std::string state);

#endif
