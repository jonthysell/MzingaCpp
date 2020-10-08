// Copyright (c) 2020 Jon Thysell <http://jonthysell.com>
// Licensed under the MIT License.

#ifndef POSITIONSET_H
#define POSITIONSET_H

#include <unordered_set>

#include "Position.h"

namespace MzingaCpp
{
	struct PositionHasher
	{
	public:
		size_t operator()(Position const& pos) const
		{
			return hash(pos);
		}
	};

	struct PositionComparator
	{
	public:
		bool operator()(Position const& lhs, Position const& rhs) const
		{
			return lhs == rhs;
		}
	};

	typedef std::unordered_set<Position, PositionHasher, PositionComparator> PositionSet;
}

#endif
