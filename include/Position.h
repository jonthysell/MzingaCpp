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

static const int NeighborDeltas[(int)Direction::NumDirections + 1][3] = {
    {0, -1, 0}, // Up
    {1, -1, 0}, // UpRight
    {1, 0, 0},  // DownRight
    {0, 1, 0},  // Down
    {-1, 1, 0}, // DownLeft
    {-1, 0, 0}, // UpLeft
    {0, 0, 1},  // Above
};

bool operator==(Position const &lhs, Position const &rhs);
bool operator!=(Position const &lhs, Position const &rhs);

size_t hash(Position const &pos);
} // namespace MzingaCpp

#endif
