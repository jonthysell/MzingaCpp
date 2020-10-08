// Copyright (c) 2020 Jon Thysell <http://jonthysell.com>
// Licensed under the MIT License.

#include <sstream>
#include <string>

#include "Constants.h"
#include "Move.h"

namespace MzingaCpp
{
	bool operator==(Move const& lhs, Move const& rhs)
	{
		return lhs.PieceName == rhs.PieceName && lhs.Source == rhs.Source && lhs.Destination == rhs.Destination;
	}

	bool operator!=(Move const& lhs, Move const& rhs)
	{
		return !(lhs == rhs);
	}

	size_t hash(Move const& move)
	{
		size_t value = 17;
		value = value * 31 + (int)move.PieceName;
		value = value * 31 + hash(move.Source);
		value = value * 31 + hash(move.Destination);
		return value;
	}

	std::string BuildMoveString(bool& isPass, PieceName& startPiece, char& beforeSeperator, PieceName& endPiece, char& afterSeperator)
	{
		if (isPass)
		{
			return PassMoveString;
		}

		std::ostringstream resultStream;

		resultStream << GetEnumString(startPiece);

		if (endPiece != PieceName::INVALID)
		{
			resultStream << " ";
			if (beforeSeperator != '\0')
			{
				resultStream << beforeSeperator << GetEnumString(endPiece);
			}
			else if (afterSeperator != '\0')
			{
				resultStream << GetEnumString(endPiece) << afterSeperator;
			}
			else
			{
				resultStream << GetEnumString(endPiece);
			}
		}

		return resultStream.str();
	}

	bool TryNormalizeMoveString(std::string const& moveString, std::string& result)
	{
		bool isPass;
		PieceName startPiece;
		char beforeSeperator;
		PieceName endPiece;
		char afterSeperator;

		if (TryNormalizeMoveString(moveString, isPass, startPiece, beforeSeperator, endPiece, afterSeperator))
		{
			result = BuildMoveString(isPass, startPiece, beforeSeperator, endPiece, afterSeperator);
			return true;
		}

		result = "";
		return false;
	}

	bool TryNormalizeMoveString(std::string const& moveString, bool& isPass, PieceName& startPiece, char& beforeSeperator, PieceName& endPiece, char& afterSeperator)
	{
		isPass = false;
		startPiece = PieceName::INVALID;
		beforeSeperator = '\0';
		endPiece = PieceName::INVALID;
		afterSeperator = '\0';

		std::ostringstream piece1;
		std::ostringstream piece2;

		int itemsFound = 0;
		for (int i = 0; i < moveString.length(); i++)
		{
			if (itemsFound == 0 && moveString[i] != ' ')
			{
				// Start of piece1, save and bump
				piece1 << moveString[i];
				itemsFound++;
			}
			else if (itemsFound == 1)
			{
				if (moveString[i] != ' ')
				{
					// Still part of piece1
					piece1 << moveString[i];
				}
				else
				{
					// Skip, looking for beforeSeperator now
					itemsFound++;
				}
			}
			else if (itemsFound == 2)
			{
				if (moveString[i] != ' ')
				{
					// First non-space
					if (moveString[i] == '-' || moveString[i] == '/' || moveString[i] == '\\')
					{
						// beforeSeparator found
						beforeSeperator = moveString[i];
					}
					else
					{
						// Start of piece2
						piece2 << moveString[i];
					}
					itemsFound++;
				}
			}
			else if (itemsFound == 3)
			{
				if (moveString[i] != ' ')
				{
					if (moveString[i] == '-' || moveString[i] == '/' || moveString[i] == '\\')
					{
						// afterSeperator found
						afterSeperator = moveString[i];
						break;
					}
					else
					{
						// Still of piece2
						piece2 << moveString[i];
					}
				}
				else
				{
					break;
				}
			}
		}

		std::string piece1Str = piece1.str();

		if (strcmpi(piece1Str.c_str(), PassMoveString) == 0)
		{
			isPass = true;
			return true;
		}

		startPiece = GetPieceNameValue(piece1Str.c_str());

		if (startPiece != PieceName::INVALID)
		{
			std::string piece2Str = piece2.str();
			endPiece = GetPieceNameValue(piece2Str.c_str());

			if (endPiece == PieceName::INVALID)
			{
				beforeSeperator = '\0';
				afterSeperator = '\0';
			}
			else if (beforeSeperator != '\0')
			{
				afterSeperator = '\0';
			}

			return true;
		}

		isPass = false;
		startPiece = PieceName::INVALID;
		beforeSeperator = '\0';
		endPiece = PieceName::INVALID;
		afterSeperator = '\0';
		return false;
	}
}
