#ifndef _FLOOR_MAP_
#define _FLOOR_MAP_

#include "position.hpp"

namespace Map {
    void reset ();
    void set_obstructed (Position pos);
    void set_clear (Position pos);
    bool is_obstructed (Position pos);
    bool is_clear (Position pos);
}

#endif