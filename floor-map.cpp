#include "floor-map.hpp"
#include "database-access.hpp"

static bool obstructed[10][10];

namespace Map {
    void reset () {
        for (int row=0; row<10; row++) {
            for (int column=0; column<10; column++) {
                if ((row == 0) || (row == 9) || (column == 0) || (column == 9)) {
                    obstructed[row][column] = true;
                } else {
                    obstructed[row][column] = false;
                }
            }
        }
        for (std::vector<int>::iterator it; it != Database::stock_get_id_list().end(); it++) {
            Map::set_obstructed(Database::stock_get_position(*it));
        }
    }

    void set_obstructed (Position pos) {
        obstructed[pos.row][pos.column] = true;
    }

    void set_clear (Position pos) {
        obstructed[pos.row][pos.column] = false;
    }

    bool is_obstructed (Position pos) {
        return obstructed[pos.row][pos.column];
    }

    bool is_clear (Position pos) {
        return !obstructed[pos.row][pos.column];
    }
}