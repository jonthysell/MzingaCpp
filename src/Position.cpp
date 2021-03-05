// Copyright (c) Jon Thysell <http://jonthysell.com>
// Licensed under the MIT License.

#include "Position.h"

using namespace MzingaCpp;

Position Position::GetNeighborAt(Direction const &direction) const
{
    return Position{Q + NeighborDeltas[(int)direction][0], R + NeighborDeltas[(int)direction][2], Stack};
}

Position Position::GetAbove() const
{
    return Position{Q, R, Stack + 1};
}

Position Position::GetBelow() const
{
    return Position{Q, R, Stack - 1};
}

Position Position::GetBottom() const
{
    return Position{Q, R, 0};
}

namespace MzingaCpp
{
bool operator==(Position const &lhs, Position const &rhs)
{
    return lhs.Q == rhs.Q && lhs.R == rhs.R && lhs.Stack == rhs.Stack;
}

bool operator!=(Position const &lhs, Position const &rhs)
{
    return !(lhs == rhs);
}

bool operator<(Position const &lhs, Position const &rhs)
{
    if (lhs.Q != rhs.Q)
    {
        return lhs.Q < rhs.Q;
    }

    if (lhs.R != rhs.R)
    {
        return lhs.R < rhs.R;
    }

    return lhs.Stack < rhs.Stack;
}

size_t hash(Position const &pos)
{
    size_t value = 17;
    value = value * 31 + pos.Q;
    value = value * 31 + pos.R;
    value = value * 31 + pos.Stack;
    return value;
}
} // namespace MzingaCpp
