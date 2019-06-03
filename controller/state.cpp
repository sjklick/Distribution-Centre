#include "state.hpp"

std::string StateToString(State state) {
	switch (state) {
		case State::idle:		return "idle";
		case State::yield:		return "yield";
		case State::home:		return "home";
		case State::retrieve:	return "retrieve";
		case State::stock:		return "stock";
		case State::receive:	return "receive";
		case State::ship:		return "ship";
		case State::extend:		return "extend";
		case State::retract:	return "retract";
		case State::pick:		return "pick";
		case State::place:		return "place";
		case State::extricate:	return "extricate";
		default:				return "?";
	}
}

State StringToState(std::string state) {
	if (state == "idle")			return State::idle;
	else if (state == "yield")		return State::yield;
	else if (state == "home")		return State::home;
	else if (state == "retrieve")	return State::retrieve;
	else if (state == "stock")		return State::stock;
	else if (state == "receive")	return State::receive;
	else if (state == "ship")		return State::ship;
	else if (state == "extend")		return State::extend;
	else if (state == "retract")	return State::retract;
	else if (state == "pick")		return State::pick;
	else if (state == "place")		return State::place;
	else if (state == "extricate")	return State::extricate;
	else							return State::invalid;
}
