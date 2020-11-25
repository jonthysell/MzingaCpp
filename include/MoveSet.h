// Copyright (c) 2020 Jon Thysell <http://jonthysell.com>
// Licensed under the MIT License.

#ifndef MOVESET_H
#define MOVESET_H

#include <unordered_set>

#include "Move.h"

namespace MzingaCpp
{
struct MoveHasher
{
  public:
    size_t operator()(Move const &move) const
    {
        return hash(move);
    }
};

struct MoveComparator
{
  public:
    bool operator()(Move const &lhs, Move const &rhs) const
    {
        return lhs == rhs;
    }
};

typedef std::unordered_set<Move, MoveHasher, MoveComparator> MoveSet;
} // namespace MzingaCpp

#endif
