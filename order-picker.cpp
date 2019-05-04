#include "order-picker.hpp"
#include <list>
#include <algorithm>
#include <cstdlib>

// Forward declarations.
static std::vector<Position> findPath(Position start, Position end, char map[10][10]);

OrderPicker::OrderPicker(int id, Position home) {
	this->id = id;
    this->home = home;
    target = home;
    path.clear();
    item = false;
	delivered = false;
	itemName = "";
	binId = -1;
	stockBinId = -1;
	yieldCount = 0;
	stockItemName = "";
	holdingStockItem = false;
	stockDelivered = false;
	stock = false;
}

Position OrderPicker::getPosition() {
    return db.getPickerCurrent(id);
}

State OrderPicker::getState() {
    return db.getPickerState(id);
}

bool OrderPicker::hasItem() {
	return item;
}

int OrderPicker::getTargetBinId() {
	return binId;
}

std::string OrderPicker::getItemName() {
	return itemName;
}

bool OrderPicker::processItem(int binId, Position bin, std::string itemName) {
	State state;
	state = db.getPickerState(id);
    if ((state == State::idle) || (state == State::home) || ((state == State::yield) && (item == false))) {
        target = getFacingPosition(bin);
        state = State::retrieve;
		db.setPickerState(id, state);
		this->itemName = itemName;
		this->binId = binId;
        return true;
    }
    return false;
}

void OrderPicker::stockItem(int stockBinId, Position bin, std::string stockItemName) {
	target = getFacingPosition(bin);
	this->stockBinId = stockBinId;
	this->stockItemName = stockItemName;
	stock = true;
	db.setPickerState(id, State::receive);
}

int OrderPicker::getStockBin() {
	return stockBinId;
}

std::string OrderPicker::getStockItemName() {
	return stockItemName;
}

void OrderPicker::updateStateIdle(char map[10][10]) {
	// Nothing to do here.
}

void OrderPicker::updateStateYield(char map[10][10]) {
	State state = State::yield;
	yieldCount++;
	if (yieldCount > 5) {
		state = State::extricate;
		// Pick a random position to target.
		extricate.row = rand() % 10;
		extricate.column = rand() % 10;
		switch (rand() % 4) {
			case 0:	extricate.facing = up;
			case 1: extricate.facing = down;
			case 2: extricate.facing = left;
			case 3: extricate.facing = right;
		}
	} else if (delivered || stockDelivered) {
		state = State::home;
	} else if (item) {
		state = State::ship;
	} else if (holdingStockItem) {
		state = State::stock;
	} else if (stock) {
		state = State::receive;
	} else {
		state = State::retrieve;
	}
	db.setPickerState(id, state);
}

void OrderPicker::updateStateHome(char map[10][10]) {
	State state = State::home;
	Position current;
	current = db.getPickerCurrent(id);
	if (current != home) {
		path = findPath(current, home, map);
		if (path.empty()) {
			state = State::yield;
			db.setPickerState(id, state);
		} else {
			yieldCount = 0;
			db.setPickerCurrent(id, path.back());
			map[current.row][current.column] = 'X';
		}
		return;
	}
	delivered = false;
	stockDelivered = false;
	itemName = "";
	stockItemName = "";
	binId = -1;
	stockBinId = -1;
	state = State::idle;
	db.setPickerState(id, state);
}

void OrderPicker::updateStateRetrieve(char map[10][10]) {
	State state = State::retrieve;
	Position current;
	current = db.getPickerCurrent(id);
	if (current != target) {
		path = findPath(current, target, map);
		if (path.empty()) {
			state = State::yield;
			db.setPickerState(id, state);
		} else {
			yieldCount = 0;
			db.setPickerCurrent(id, path.back());
			map[current.row][current.column] = 'X';
		}
		return;
	}
	state = State::extend;
	db.setPickerState(id, state);
}

void OrderPicker::updateStateStock(char map[10][10]) {
	State state = State::stock;
	Position current;
	current = db.getPickerCurrent(id);
	if (current != target) {
		path = findPath(current, target, map);
		if (path.empty()) {
			state = State::yield;
			db.setPickerState(id, state);
		} else {
			yieldCount = 0;
			db.setPickerCurrent(id, path.back());
			map[current.row][current.column] = 'X';
		}
		return;
	}
	state = State::extend;
	db.setPickerState(id, state);
}

void OrderPicker::updateStateReceive(char map[10][10]) {
	State state = State::receive;
	Position bin;
	bin.row = 0;
	bin.column = 8;
	bin.facing = down;
	bin = getFacingPosition(bin);
	Position current;
	current = db.getPickerCurrent(id);
	if (current != bin) {
		path = findPath(current, bin, map);
		if (path.empty()) {
			state = State::yield;
			db.setPickerState(id, state);
		} else {
			yieldCount = 0;
			db.setPickerCurrent(id, path.back());
			map[current.row][current.column] = 'X';
		}
		return;
	}
	state = State::extend;
	db.setPickerState(id, state);
}

void OrderPicker::updateStateShip(char map[10][10]) {
	State state = State::ship;
	Position bin;
	bin.row = 9;
	bin.column = 1;
	bin.facing = up;
	bin = getFacingPosition(bin);
	Position current;
	current = db.getPickerCurrent(id);
	if (current != bin) {
		path = findPath(current, bin, map);
		if (path.empty()) {
			state = State::yield;
			db.setPickerState(id, state);
		} else {
			yieldCount = 0;
			db.setPickerCurrent(id, path.back());
			map[current.row][current.column] = 'X';
		}
		return;
	}
	state = State::extend;
	db.setPickerState(id, state);
}

void OrderPicker::updateStateExtend(char map[10][10]) {
	State state = State::extend;
	if (item || holdingStockItem) state = State::place;
	else state = State::pick;
	db.setPickerState(id, state);
}

void OrderPicker::updateStateRetract(char map[10][10]) {
	State state = State::retract;
	if (item) state = State::ship;
	else if (holdingStockItem) state = State::stock;
	else state = State::home;
	db.setPickerState(id, state);
}

void OrderPicker::updateStatePick(char map[10][10]) {
	if (stock) holdingStockItem = true;
	else item = true;
	db.setPickerState(id, State::retract);
}

void OrderPicker::updateStatePlace(char map[10][10]) {
	if (stock) {
		holdingStockItem = false;
		stockDelivered = true;
	} else {
		item = false;
		delivered = true;
	}
	db.setPickerState(id, State::retract);
}

void OrderPicker::updateStateExtricate(char map[10][10]) {
	State state = State::extricate;
	Position current;
	current = db.getPickerCurrent(id);
	if (current != extricate) {
		path = findPath(current, extricate, map);
		if (path.empty()) {
			state = State::yield;
			db.setPickerState(id, state);
		} else {
			yieldCount = 0;
			db.setPickerCurrent(id, path.back());
			map[current.row][current.column] = 'X';
		}
		return;
	}
	if (delivered || stockDelivered) {
		state = State::home;
	} else if (item) {
		state = State::ship;
	} else if (holdingStockItem) {
		state = State::stock;
	} else if (stock) {
		state = State::receive;
	} else {
		state = State::retrieve;
	}
	db.setPickerState(id, state);
}

void OrderPicker::update(char map[10][10]) {
	switch (OrderPicker::getState()) {
		case State::yield:		updateStateYield(map);		break;
		case State::home:		updateStateHome(map);		break;
		case State::retrieve:	updateStateRetrieve(map);	break;
		case State::stock:		updateStateStock(map);		break;
		case State::receive:	updateStateReceive(map);	break;
		case State::ship:		updateStateShip(map);		break;
		case State::extend:		updateStateExtend(map);		break;
		case State::retract:	updateStateRetract(map);	break;
		case State::pick:		updateStatePick(map);		break;
		case State::place:		updateStatePlace(map);		break;
		case State::extricate:	updateStateExtricate(map);	break;
		default:				updateStateIdle(map);
    }
}

OrderPicker::~OrderPicker() {
    // nothing for now
}

struct PathNode {
    Position position;
    float F, G, H;
    PathNode* parent;
};

static std::vector<Position> getValidMoves(Position current, char map[10][10]) {
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
            if (map[temp.row][temp.column] == '.') {
                moves.push_back(temp);
            }
        }
    }

    return moves;
}

static float getHeuristicDistance(Position current, Position destination) {
    // will use simple manhattan distance, ignoring rotations too
    return float(abs(destination.row-current.row) + abs(destination.column-current.column));
}

static std::vector<Position> findPath(Position start, Position end, char map[10][10]) {
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
        std::vector<Position> moves = getValidMoves(current.position, map);
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
