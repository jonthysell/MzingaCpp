// Copyright (c) 2020 Jon Thysell <http://jonthysell.com>
// Licensed under the MIT License.

#include <sstream>

#include "Board.h"

using namespace MzingaCpp;

#define GameInProgress (m_boardState == BoardState::NotStarted || m_boardState == BoardState::InProgress)

Board::Board()
{
	for (int pn = 1; pn < (int)PieceName::NumPieceNames; pn++)
	{
		m_piecePositions[pn] = Position{ 0, 0, -pn };
	}
}

std::string Board::GetGameString()
{
	std::ostringstream str;

	str << "Base";
	str << ";" << GetEnumString(m_boardState);
	str << ";" << GetEnumString(m_currentColor) << "[" << m_currentTurn + 1 << "]";

	for (auto const& iter : m_moveHistoryStr)
	{
		str << ";" << iter;
	}

	return str.str();
}

std::shared_ptr<MoveSet> Board::GetValidMoves()
{
	if (nullptr == m_cachedValidMoves)
	{
		m_cachedValidMoves = std::make_shared<MoveSet>();

		if (GameInProgress)
		{
			for (int pn = 1; pn < (int)PieceName::NumPieceNames; pn++)
			{
				GetValidMoves((PieceName)pn, m_cachedValidMoves);
			}

			if (m_cachedValidMoves->size() == 0)
			{
				m_cachedValidMoves->insert(PassMove);
			}
		}
	}

	return m_cachedValidMoves;
}

bool Board::TryPlayMove(Move const& move, std::string moveString)
{
	auto validMoves = GetValidMoves();

	auto it = validMoves->find(move);
	if (it != validMoves->end())
	{
		m_moveHistory.push_back(move);

		if (moveString.empty())
		{
			TryGetMoveString(move, moveString);
		}

		m_moveHistoryStr.push_back(moveString);

		if (move != PassMove)
		{
			m_piecePositions[(int)move.PieceName] = move.Destination;
		}

		m_currentTurn++;
		m_currentColor = (Color)(m_currentTurn % (int)Color::NumColors);

		ResetCaches();

		return true;
	}

	return false;
}

bool Board::TryGetMoveString(Move const& move, std::string& result)
{
	if (move == PassMove)
	{
		result = PassMoveString;
		return true;
	}

	std::string startPiece = GetEnumString(move.PieceName);

	if (m_currentTurn == 0 && move.Destination == OriginPosition)
	{
		result = startPiece;
		return true;
	}

	std::string endPiece = "";

	if (move.Destination.Stack > 0)
	{
		PieceName pieceBelow = GetPieceAt(move.Destination.GetBelow());
		endPiece = GetEnumString(pieceBelow);
	}
	else
	{
		for (int dir = 0; dir < (int)Direction::NumDirections; dir++)
		{
			Position neighborPosition = move.Destination.GetNeighborAt((Direction)dir);
			PieceName neighbor = GetPieceAt(neighborPosition);

			if (neighbor != PieceName::INVALID && neighbor != move.PieceName)
			{
				endPiece = GetEnumString(neighbor);
				switch (dir)
				{
				case 0: // Up
					endPiece = endPiece + "\\";
					break;
				case 1:  // UpRight
					endPiece = "/" + endPiece;
					break;
				case 2:  // DownRight
					endPiece = "-" + endPiece;
					break;
				case 3: // Down
					endPiece = "\\" + endPiece;
					break;
				case 4: // DownLeft
					endPiece = endPiece + "/";
					break;
				case 5: // UpLeft
					endPiece = endPiece + "-";
					break;
				}
				break;
			}
		}
	}

	if (!endPiece.empty())
	{
		result = (startPiece + " ") + endPiece;
		return true;
	}

	return false;
}

bool Board::TryParseMove(std::string moveString, Move& result, std::string& resultString)
{
	bool isPass;
	PieceName startPiece;
	char beforeSeperator;
	PieceName endPiece;
	char afterSeperator;

	if (TryNormalizeMoveString(moveString, isPass, startPiece, beforeSeperator, endPiece, afterSeperator))
	{
		resultString = BuildMoveString(isPass, startPiece, beforeSeperator, endPiece, afterSeperator);

		if (isPass)
		{
			result = PassMove;
			return true;
		}

		Position source = m_piecePositions[(int)startPiece];

		Position destination = OriginPosition;

		if (endPiece != PieceName::INVALID)
		{
			Position targetPosition = m_piecePositions[(int)endPiece];

			if (beforeSeperator != '\0')
			{
				// Moving piece on the left-hand side of the target piece
				switch (beforeSeperator)
				{
				case '-':
					destination = targetPosition.GetNeighborAt(Direction::UpLeft);
					break;
				case '/':
					destination = targetPosition.GetNeighborAt(Direction::DownLeft);
					break;
				case '\\':
					destination = targetPosition.GetNeighborAt(Direction::Up);
					break;
				}
			}
			else if (afterSeperator != '\0')
			{
				// Moving piece on the right-hand side of the target piece
				switch (afterSeperator)
				{
				case '-':
					destination = targetPosition.GetNeighborAt(Direction::DownRight);
					break;
				case '/':
					destination = targetPosition.GetNeighborAt(Direction::UpRight);
					break;
				case '\\':
					destination = targetPosition.GetNeighborAt(Direction::Down);
					break;
				}
			}
			else
			{
				destination = targetPosition.GetAbove();
			}
		}

		result = Move{ startPiece, source, destination };
		return true;
	}

	result = Move{};
	return false;
}

void Board::GetValidMoves(PieceName const& pieceName, std::shared_ptr<MoveSet> moveSet)
{
	if (pieceName != PieceName::INVALID && GameInProgress)
	{
		if (m_currentTurn == 0)
		{
			// First turn by white
			switch (pieceName)
			{
			case PieceName::wS1:
			case PieceName::wB1:
			case PieceName::wG1:
			case PieceName::wA1:
				moveSet->insert(Move{ pieceName, m_piecePositions[(int)pieceName], OriginPosition });
				break;
			}
		}
		else if (m_currentTurn == 1)
		{
			// First turn by black
			switch (pieceName)
			{
			case PieceName::bS1:
			case PieceName::bB1:
			case PieceName::bG1:
			case PieceName::bA1:
				auto validPlacements = GetValidPlacements();
				for (auto const& iter : *validPlacements)
				{
					moveSet->insert(Move{ pieceName, m_piecePositions[(int)pieceName], iter });
				}
				break;
			}
		}
	}
}

std::shared_ptr<PositionSet> Board::GetValidPlacements()
{
	if (nullptr == m_cachedValidPlacements)
	{
		m_cachedValidPlacements = std::make_shared<PositionSet>();

		if (m_currentTurn == 0)
		{
			m_cachedValidPlacements->insert(OriginPosition);
		}
		else if (m_currentTurn == 1)
		{
			for (int dir = 0; dir < (int)Direction::NumDirections; dir++)
			{
				m_cachedValidPlacements->insert(OriginPosition.GetNeighborAt((Direction)dir));
			}
		}
	}
	return m_cachedValidPlacements;
}

PieceName Board::GetPieceAt(Position const& position)
{
	for (int pn = 1; pn < (int)PieceName::NumPieceNames; pn++)
	{
		if (m_piecePositions[pn] == position)
		{
			return (PieceName)pn;
		}
	}

	return PieceName::INVALID;
}

void Board::ResetCaches()
{
	m_cachedValidPlacements = nullptr;
	m_cachedValidMoves = nullptr;
}
