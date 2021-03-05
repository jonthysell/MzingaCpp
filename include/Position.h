// Copyright (c) Jon Thysell <http://jonthysell.com>
// Licensed under the MIT License.

#ifndef POSITION_H
#define POSITION_H

#include "Enums.h"

namespace MzingaCpp
{
struct Position
{
    int Q;
    int R;
    int Stack;

  public:
    Position GetNeighborAt(Direction const &direction) const;
    Position GetAbove() const;
    Position GetBelow() const;
    Position GetBottom() const;
};

static const Position OriginPosition{0, 0, 0};

static const Position NullPosition{0, 0, -1};

static const int NeighborDeltas[(int)Direction::NumDirections][3] = {
    {0, 1, -1}, {1, 0, -1}, {1, -1, 0}, {0, -1, 1}, {-1, 0, 1}, {-1, 1, 0},
};

bool operator==(Position const &lhs, Position const &rhs);
bool operator!=(Position const &lhs, Position const &rhs);
bool operator<(Position const &lhs, Position const &rhs);

size_t hash(Position const &pos);
} // namespace MzingaCpp

#endif
