#include <iostream>
#include <cstring>

enum Direction { up, down, left, right };

struct Position {
    int row;
    int column;
    Direction facing;
};

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

    // map copy for display
    char display_map[10][10];
    memcpy(display_map, init_map, 100);

    // testing start and end points
    Position start, end;
    start.row = 1;
    start.column = 1;
    start.facing = down;
    end.row = 7;
    end.column = 5;
    end.facing = left;

    // add start and end points to display map
    display_map[start.row][start.column] = DirectionToChar(start.facing);
    display_map[end.row][end.column] = DirectionToChar(end.facing);

    // print map
    for (int row=0; row<10; row++) {
        for (int column=0; column<10; column++) {
            std::cout << display_map[row][column];
        }
        std::cout << std::endl;
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
