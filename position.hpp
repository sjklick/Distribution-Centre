#ifndef __POSITION__
#define __POSITION__

enum Direction { up, down, left, right };

struct Position {
    int row;
    int column;
    Direction facing;
};

bool operator==(const Position &lhs, const Position &rhs) {
    if (lhs.row == rhs.row) {
        if (lhs.column == rhs.column) {
            if (lhs.facing == rhs.facing) {
                return true;
            }
        }
    }
    return false;
}

#endif
