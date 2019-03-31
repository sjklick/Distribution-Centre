#include <iostream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <list>
#include <algorithm>
#include "position.hpp"

struct PathNode {
    Position position;
    float F, G, H;
    PathNode* parent;
};

// function prototypes
char DirectionToChar (Direction dir);
std::vector<Position> getValidMoves(Position current, char map[10][10]);
float getHeuristicDistance(Position current, Position destination);
std::vector<Position> getPath(Position start, Position end, char map[10][10]);

int main() {
    // initial warehouse map
    // 'X' => wall, 'B' => bin, '.' => empty
    char init_map[10][10] = {{'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'},
                             {'X', '.', '.', '.', '.', '.', '.', '.', '.', 'X'},
                             {'X', '.', '.', 'B', 'B', '.', 'B', 'B', '.', 'X'},
                             {'X', '.', '.', 'B', 'B', '.', 'B', 'B', '.', 'X'},
                             {'X', '.', '.', 'B', 'B', '.', 'B', 'B', '.', 'X'},
                             {'X', '.', '.', '.', '.', '.', 'B', 'B', '.', 'X'},
                             {'X', '.', '.', 'B', 'B', '.', 'B', 'B', '.', 'X'},
                             {'X', '.', '.', 'B', 'B', '.', 'B', 'B', '.', 'X'},
                             {'X', '.', '.', '.', '.', '.', '.', '.', '.', 'X'},
                             {'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'}};
    char display_map[10][10];

    // a test path => delete later
    std::vector<Position> path;
    Position start, end;
    start.row = 1;
    start.column = 1;
    start.facing = down;
    end.row = 7;
    end.column = 5;
    end.facing = left;
    path = getPath(start, end, init_map);

    char c;
    for (std::vector<Position>::iterator it = path.begin(); it != path.end(); it++) {

        // map copy for display
        memcpy(display_map, init_map, 100);

        // add current position to display map
        display_map[(*it).row][(*it).column] = DirectionToChar((*it).facing);

        // print map
        system("clear");
        for (int row=0; row<10; row++) {
            for (int column=0; column<10; column++) {
                std::cout << display_map[row][column];
            }
            std::cout << std::endl;
        }

        std::cin.get(c);

    }
}

char DirectionToChar (Direction dir) {
    switch (dir) {
        case up:    return 'u';
        case down:  return 'd';
        case left:  return 'l';
        case right: return 'r';
        default:    return '?';
    }
}

std::vector<Position> getValidMoves(Position current, char map[10][10]) {
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

float getHeuristicDistance(Position current, Position destination) {
    // will use simple manhattan distance, ignoring rotations too
    return float(abs(destination.row-current.row) + abs(destination.column-current.column));
}

std::vector<Position> getPath(Position start, Position end, char map[10][10]) {
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
        if (current.position == end) break;

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

        // if the open list is empty, and we are here, then there were no new nodes
        // to try and no path to destination was found
        if (openList.empty()) break;

    }

    // Save the found path to the path vector. Start with the target square, going
    // through the parents until start node is reached.
    if (!openList.empty()) {
        // If we are here, a path was found (open list was not empty), and the target
        // node was just pushed onto the back of the closed list.
        temp = closedList.back();
        while (temp.parent != NULL) {
            temp_pos = temp.position;
            path.push_back(temp_pos);
            temp = *(temp.parent);
        }
        temp_pos = temp.position;
        path.push_back(temp_pos);
        std::reverse(path.begin(), path.end());
    }

    return path;
}
