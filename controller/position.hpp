#ifndef __POSITION__
#define __POSITION__

enum Direction { up, down, left, right, invalid};

struct Position {
    int row;
    int column;
    Direction facing;
};

bool operator==(const Position &lhs, const Position &rhs);

bool operator!=(const Position &lhs, const Position &rhs);

Position getFacingPosition(Position pos);

char DirectionToChar(Direction dir);

Direction CharToDirection(char c);

#endif
