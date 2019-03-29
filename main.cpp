#include <iostream>
#include <cstring>
#include <cstdlib>
#include <vector>

enum Direction { up, down, left, right };

struct Position {
    int row;
    int column;
    Direction facing;
};

// function prototypes
char DirectionToChar (Direction dir);

int main() {
    // initial warehouse map
    // 'X' => wall, 'B' => bin, '.' => empty
    const char init_map[10][10] = {{'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'},
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
    Position next;
    next.row = 1;
    next.column = 1;
    next.facing = down;
    path.push_back(next);
    next.row = 2;
    next.column = 1;
    next.facing = down;
    path.push_back(next);
    next.row = 3;
    next.column = 1;
    next.facing = down;
    path.push_back(next);
    next.row = 4;
    next.column = 1;
    next.facing = down;
    path.push_back(next);
    next.row = 5;
    next.column = 1;
    next.facing = down;
    path.push_back(next);
    next.row = 5;
    next.column = 1;
    next.facing = right;
    path.push_back(next);
    next.row = 5;
    next.column = 2;
    next.facing = right;
    path.push_back(next);
    next.row = 5;
    next.column = 3;
    next.facing = right;
    path.push_back(next);
    next.row = 5;
    next.column = 4;
    next.facing = right;
    path.push_back(next);
    next.row = 5;
    next.column = 5;
    next.facing = right;
    path.push_back(next);
    next.row = 5;
    next.column = 5;
    next.facing = down;
    path.push_back(next);
    next.row = 6;
    next.column = 5;
    next.facing = down;
    path.push_back(next);
    next.row = 7;
    next.column = 5;
    next.facing = down;
    path.push_back(next);
    next.row = 7;
    next.column = 5;
    next.facing = left;
    path.push_back(next);

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
