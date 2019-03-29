#include <iostream>
#include <cstring>
using namespace std;

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
    int start_row = 1;
    int start_column = 1;
    int end_row = 7;
    int end_column = 5;

    // add start and end points to display map
    // 'S' => start, 'E' => end
    display_map[start_row][start_column] = 'S';
    display_map[end_row][end_column] = 'E';

    // print map
    for (int row=0; row<10; row++) {
        for (int column=0; column<10; column++) {
            cout << display_map[row][column];
        }
        cout << endl;
    }
}
