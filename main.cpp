#include <iostream>
#include <cstring>
#include <cstdlib>
#include "position.hpp"
#include "order-picker.hpp"

char DirectionToChar (Direction dir) {
    switch (dir) {
        case up:    return 'u';
        case down:  return 'd';
        case left:  return 'l';
        case right: return 'r';
        default:    return '?';
    }
}

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
    Position home, bin;
    home.row = 1;
    home.column = 1;
    home.facing = down;
    bin.row = 7;
    bin.column = 4;
    bin.facing = right;
    OrderPicker op(home);

    if (op.processItem(bin)) {
        char c;
        Position current;
        while (op.getState() == State::retrieve) {
            // map copy for display
            memcpy(display_map, init_map, 100);
            // add current position to display map
            current = op.getPosition();
            display_map[current.row][current.column] = DirectionToChar(current.facing);
            // print map
            system("clear");
            for (int row=0; row<10; row++) {
                for (int column=0; column<10; column++) {
                    std::cout << display_map[row][column];
                }
                std::cout << std::endl;
            }
            // wait for user to enter char
            std::cin.get(c);
            op.update(init_map);
        }
    }
}
