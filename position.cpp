#include "position.hpp"

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

bool operator!=(const Position &lhs, const Position &rhs) {
    if (lhs.row == rhs.row) {
        if (lhs.column == rhs.column) {
            if (lhs.facing == rhs.facing) {
                return false;
            }
        }
    }
    return true;
}

Position getFacingPosition(Position pos) {
    Position temp;
    switch (pos.facing) {
        case up:    temp.facing = down;
                    temp.row = pos.row - 1;
                    temp.column = pos.column;
                    break;
        case down:  temp.facing = up;
                    temp.row = pos.row + 1;
                    temp.column = pos.column;
                    break;
        case left:  temp.facing = right;
                    temp.row = pos.row;
                    temp.column = pos.column - 1;
                    break;
        case right: temp.facing = left;
                    temp.row = pos.row;
                    temp.column = pos.column + 1;
                    break;
    }
    return temp;
}
