#include "floor-map.hpp"
#include "order-picker.hpp"
#include "database-access.hpp"
#include <list>
#include <algorithm>
#include <cstdlib>

// Forward declarations.
static std::vector<Position> findPath(Position start, Position end);

static void updateStateIdle (int pickerId) {
	// Nothing to do here.
}

static void updateStateYield (int pickerId) {
	State state = State::yield;
	Database::picker_increment_yield_count(pickerId);
	if (Database::picker_get_yield_count(pickerId) > 5) {
		state = State::extricate;
		// Pick a random position to target.
		Position extricate;
		extricate.row = rand() % 10;
		extricate.column = rand() % 10;
		switch (rand() % 4) {
			case 0:	extricate.facing = up;
			case 1: extricate.facing = down;
			case 2: extricate.facing = left;
			case 3: extricate.facing = right;
		}
		Database::picker_set_target(pickerId, extricate);
		Database::picker_set_state(pickerId, state);
	}
	if (Database::picker_is_task_complete(pickerId)) {
		state = State::home;
	} else if (Database::picker_has_item(pickerId)) {
		if (Database::picker_is_task_ship(pickerId)) {
			state = State::ship;
		} else if (Database::picker_is_task_receive(pickerId)) {
			state = State::stock;
		}
	} else if (!Database::picker_has_item(pickerId)) {
		if (Database::picker_is_task_ship(pickerId)) {
			state = State::retrieve;
		} else if (Database::picker_is_task_receive(pickerId)) {
			state = State::receive;
		}
	}
	Database::picker_set_state(pickerId, state);
}

static void updateStateHome (int pickerId) {
	State state = State::home;
	Position current, home;
	current = Database::picker_get_current(pickerId);
	home = Database::picker_get_home(pickerId);
	if (current != home) {
		std::vector<Position> path = findPath(current, home);
		if (path.empty()) {
			state = State::yield;
			Database::picker_set_state(pickerId, state);
		} else {
			Database::picker_reset_yield_count(pickerId);
			Database::picker_set_current(pickerId, path.back());
			Map::set_obstructed(current);
		}
		return;
	}
	state = State::idle;
	Database::picker_set_state(pickerId, state);
}

static void updateStateRetrieve (int pickerId) {
	State state = State::retrieve;
	Position current, target;
	current = Database::picker_get_current(pickerId);
	target = Database::picker_get_target(pickerId);
	if (current != target) {
		std::vector<Position> path = findPath(current, target);
		if (path.empty()) {
			state = State::yield;
			Database::picker_set_state(pickerId, state);
		} else {
			Database::picker_reset_yield_count(pickerId);
			Database::picker_set_current(pickerId, path.back());
			Map::set_obstructed(current);
		}
		return;
	}
	state = State::extend;
	Database::picker_set_state(pickerId, state);
}

static void updateStateStock (int pickerId) {
	State state = State::stock;
	Position current, target;
	current = Database::picker_get_current(pickerId);
	target = Database::picker_get_target(pickerId);
	if (current != target) {
		std::vector<Position> path = findPath(current, target);
		if (path.empty()) {
			state = State::yield;
			Database::picker_set_state(pickerId, state);
		} else {
			Database::picker_reset_yield_count(pickerId);
			Database::picker_set_current(pickerId, path.back());
			Map::set_obstructed(current);
		}
		return;
	}
	state = State::extend;
	Database::picker_set_state(pickerId, state);
}

static void updateStateReceive (int pickerId) {
	State state = State::receive;
	Position bin;
	bin.row = 0;
	bin.column = 8;
	bin.facing = down;
	bin = getFacingPosition(bin);
	Position current;
	current = Database::picker_get_current(pickerId);
	if (current != bin) {
		std::vector<Position> path = findPath(current, bin);
		if (path.empty()) {
			state = State::yield;
			Database::picker_set_state(pickerId, state);
		} else {
			Database::picker_reset_yield_count(pickerId);
			Database::picker_set_current(pickerId, path.back());
			Map::set_obstructed(current);
		}
		return;
	}
	state = State::extend;
	Database::picker_set_state(pickerId, state);
}

static void updateStateShip (int pickerId) {
	State state = State::ship;
	Position bin;
	bin.row = 9;
	bin.column = 1;
	bin.facing = up;
	bin = getFacingPosition(bin);
	Position current;
	current = Database::picker_get_current(pickerId);
	if (current != bin) {
		std::vector<Position> path = findPath(current, bin);
		if (path.empty()) {
			state = State::yield;
			Database::picker_set_state(pickerId, state);
		} else {
			Database::picker_reset_yield_count(pickerId);
			Database::picker_set_current(pickerId, path.back());
			Map::set_obstructed(current);
		}
		return;
	}
	state = State::extend;
	Database::picker_set_state(pickerId, state);
}

static void updateStateExtend (int pickerId) {
	State state = State::extend;
	if (Database::picker_has_item(pickerId)) state = State::place;
	else state = State::pick;
	Database::picker_set_state(pickerId, state);
}

static void updateStateRetract (int pickerId) {
	State state = State::retract;
	if (!Database::picker_has_item(pickerId)) {
		state = State::home;
	} else {
		if (Database::picker_is_task_ship(pickerId)) {
			state = State::ship;
		} else if (Database::picker_is_task_receive(pickerId)) {
			state = State::stock;
		}
	}
	Database::picker_set_state(pickerId, state);
}

static void updateStatePick (int pickerId) {
	if (Database::picker_is_task_ship(pickerId)) {
		Database::picker_take_item_from_stock(pickerId);
	} else if (Database::picker_is_task_receive(pickerId)) {
		Database::picker_take_item_from_receiving(pickerId);
	}
	Database::picker_set_state(pickerId, State::retract);
}

static void updateStatePlace (int pickerId) {
	if (Database::picker_is_task_ship(pickerId)) {
		Database::picker_place_item_into_stock(pickerId);
	} else if (Database::picker_is_task_receive(pickerId)) {
		Database::picker_place_item_into_shipping(pickerId);
	}
	Database::picker_set_state(pickerId, State::retract);
}

static void updateStateExtricate (int pickerId) {
	State state = State::extricate;
	Position current, extricate;
	current = Database::picker_get_current(pickerId);
	extricate = Database::picker_get_target(pickerId);
	if (current != extricate) {
		std::vector<Position> path = findPath(current, extricate);
		if (path.empty()) {
			state = State::yield;
			Database::picker_set_state(pickerId, state);
		} else {
			Database::picker_reset_yield_count(pickerId);
			Database::picker_set_current(pickerId, path.back());
			Map::set_obstructed(current);
		}
		return;
	}
	if (Database::picker_is_task_complete(pickerId)) {
		state = State::home;
	} else if (Database::picker_has_item(pickerId)) {
		if (Database::picker_is_task_ship(pickerId)) {
			state = State::ship;
		} else if (Database::picker_is_task_receive(pickerId)) {
			state = State::stock;
		}
	} else if (!Database::picker_has_item(pickerId)) {
		if (Database::picker_is_task_ship(pickerId)) {
			state = State::retrieve;
		} else if (Database::picker_is_task_receive(pickerId)) {
			state = State::receive;
		}
	}
	Database::picker_set_state(pickerId, state);
}

struct PathNode {
    Position position;
    float F, G, H;
    PathNode* parent;
};

static std::vector<Position> getValidMoves (Position current) {
    std::vector<Position> moves;
    Position temp;
    
    // start by adding valid rotations
    temp.row = current.row;
    temp.column = current.column;
    switch (current.facing) {
        case up: case down:
            temp.facing = left;
            moves.push_back(temp);
            temp.facing = right;
            moves.push_back(temp);
            break;
        case left: case right:
            temp.facing = up;
            moves.push_back(temp);
            temp.facing = down;
            moves.push_back(temp);
            break;
    }

    // add moving forward, if valid
    temp.facing = current.facing;
    switch (current.facing) {
        case up:
            temp.row -= 1;
            break;
        case down:
            temp.row += 1;
            break;
        case left:
            temp.column -= 1;
            break;
        case right:
            temp.column += 1;
            break;
    }
    if ((temp.row >= 0) && (temp.row <= 9)) {
        if ((temp.column >= 0) && (temp.column <= 9)) {
            if (Map::is_clear(temp)) {
                moves.push_back(temp);
            }
        }
    }

    return moves;
}

static float getHeuristicDistance (Position current, Position destination) {
    // will use simple manhattan distance, ignoring rotations too
    return float(abs(destination.row-current.row) + abs(destination.column-current.column));
}

static std::vector<Position> findPath(Position start, Position end) {
    std::list<PathNode> openList, closedList;
    std::vector<Position> path;
    PathNode temp;
    Position temp_pos;
    
    // add starting node to open list
    temp.position.row = start.row;
    temp.position.column = start.column;
    temp.position.facing = start.facing;
    temp.parent = NULL;
    temp.G = 0.0;
    temp.H = getHeuristicDistance(start, end);
    temp.F = temp.G + temp.H;
    openList.push_back(temp);

    while (true) {
        // get current square and move to closed list
        PathNode current;
        std::list<PathNode>::iterator current_it = openList.begin();
        current = *current_it;
        for (std::list<PathNode>::iterator it = openList.begin(); it != openList.end(); it++) {
            if ((*current_it).F > (*it).F) {
                current_it = it;
                current = *it;
            }
        }
        closedList.push_back(current);
        openList.erase(current_it);

        // stop if the end node was just placed on the closed list (i.e. path found)
        if (current.position == end) {
			// Save the found path to the path vector. Start with the target square, going
			// through the parents until start node is reached.
			temp = closedList.back();
        	while (temp.parent != NULL) {
		        temp_pos = temp.position;
		        path.push_back(temp_pos);
		        temp = *(temp.parent);
		    }
			// Go return our populated path.
			break;
		}

        // for each valid move from current position...
        std::vector<Position> moves = getValidMoves(current.position);
        bool onClosedList, onOpenList;
        for (std::vector<Position>::iterator it = moves.begin(); it != moves.end(); it++) {
            // check if the move is not on the closed list
            onClosedList = false;
            for (std::list<PathNode>::iterator closed_it = closedList.begin(); closed_it != closedList.end(); closed_it++) {
                if ((*closed_it).position == (*it)) {
                    onClosedList = true;
                    break;
                }
            }
            if (!onClosedList) {
                // now check if the move is on the open list
                std::list<PathNode>::iterator found_it;
                onOpenList = false;
                for (std::list<PathNode>::iterator open_it = openList.begin(); open_it != openList.end(); open_it++) {
                    if ((*open_it).position == (*it)) {
                        found_it = open_it;
                        onOpenList = true;
                        break;
                    }
                }
                temp.position.row = (*it).row;
                temp.position.column = (*it).column;
                temp.position.facing = (*it).facing;
                // Be careful here. Remember the scope of the "current" variable. What we really want is the address
                // of the persistent PathNode representing current, which is stored at the end of closedList right now.
                temp.parent = &(closedList.back());
                temp.G = current.G + 1.0;
                temp.H = getHeuristicDistance(temp.position, end);
                temp.F = temp.G + temp.H;
                if (!onOpenList) {
                    // add move to open list, making current node the parent
                    openList.push_back(temp);
                } else {
                    // check if path to this position is better than the path
                    // used to get to this position, as stored on the open list.
                    // Replace if better.
                    if ((*found_it).G > temp.G) {
                        openList.erase(found_it);
                        openList.push_back(temp);
                    }
                }
            }
        }

        // Ff the open list is empty, and we are here, then there were no new nodes
        // to try and no path to destination was found (return empty path)
        if (openList.empty()) break;

    }

    return path;
}

namespace Picker {
	bool is_assigned (int pickerId) {
		return Database::picker_check_if_assigned(pickerId);
	}

	void assign_shipping_task (int pickerId, std::string item, int bin) {
		Database::picker_assign_shipping_task(pickerId, item, bin);
	}

	void assign_receiving_task (int pickerId, std::string item, int bin) {
		Database::picker_assign_receiving_task(pickerId, item, bin);
	}

	void update (int pickerId) {
		State state = Database::picker_get_state(pickerId);
		switch (state) {
			case State::yield:		updateStateYield(pickerId);		break;
			case State::home:		updateStateHome(pickerId);		break;
			case State::retrieve:	updateStateRetrieve(pickerId);	break;
			case State::stock:		updateStateStock(pickerId);		break;
			case State::receive:	updateStateReceive(pickerId);	break;
			case State::ship:		updateStateShip(pickerId);		break;
			case State::extend:		updateStateExtend(pickerId);	break;
			case State::retract:	updateStateRetract(pickerId);	break;
			case State::pick:		updateStatePick(pickerId);		break;
			case State::place:		updateStatePlace(pickerId);		break;
			case State::extricate:	updateStateExtricate(pickerId);	break;
			default:				updateStateIdle(pickerId);
		}
	}

	Position get_position (int pickerId) {
		return Database::picker_get_current(pickerId);
	}
}