// Copyright (c) 2020 Jon Thysell <http://jonthysell.com>
// Licensed under the MIT License.

#include <sstream>
#include <assert.h>

#include "Board.h"

using namespace MzingaCpp;

#define GameInProgress (m_boardState == BoardState::NotStarted || m_boardState == BoardState::InProgress)
#define CurrentPlayerTurn (1 + m_currentTurn / 2)

#define CurrentTurnQueenInPlay PieceInPlay(m_currentColor == Color::White ? PieceName::wQ : PieceName::bQ)

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
	str << ";" << GetEnumString(m_currentColor) << "[" << CurrentPlayerTurn << "]";

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
		m_boardState = m_currentTurn == 0 ? BoardState::NotStarted : BoardState::InProgress;

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
	if (pieceName != PieceName::INVALID && GameInProgress && m_currentColor == GetColor(pieceName) && PlacingPieceInOrder(pieceName))
	{
		int pieceIndex = (int)pieceName;

		if (m_currentTurn == 0)
		{
			// First turn by white
			if (pieceName != PieceName::wQ)
			{
				moveSet->insert(Move{ pieceName, m_piecePositions[pieceIndex], OriginPosition });
			}
		}
		else if (m_currentTurn == 1)
		{
			// First turn by black
			if (pieceName != PieceName::bQ)
			{
				auto validPlacements = GetValidPlacements();
				for (auto const& iter : *validPlacements)
				{
					moveSet->insert(Move{ pieceName, m_piecePositions[pieceIndex], iter });
				}
			}
		}
		else if (PieceInHand(pieceName))
		{
			// Piece is in hand
			if ((CurrentPlayerTurn != 4 ||
				(CurrentPlayerTurn == 4 &&
					(CurrentTurnQueenInPlay || (!CurrentTurnQueenInPlay && GetBugType(pieceName) == BugType::QueenBee)))))
			{
				auto validPlacements = GetValidPlacements();
				for (auto const& iter : *validPlacements)
				{
					moveSet->insert(Move{ pieceName, m_piecePositions[pieceIndex], iter });
				}
			}
		}
		else if (CurrentTurnQueenInPlay)
		{
			// Piece is in play and movement is allowed
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
		else
		{
			auto visitedPlacements = std::make_shared<PositionSet>();

			for (int pn = 1; pn < (int)PieceName::NumPieceNames; pn++)
			{
				auto pieceName = (PieceName)pn;

				if (PieceIsOnTop(pieceName) && m_currentColor == GetColor(pieceName))
				{
					auto bottomPosition = m_piecePositions[pn].GetBottom();
					visitedPlacements->insert(bottomPosition);

					for (int dir = 0; dir < (int)Direction::NumDirections; dir++)
					{
						auto neighbor = bottomPosition.GetNeighborAt((Direction)dir);

						if (visitedPlacements->find(neighbor) == visitedPlacements->end() && !HasPieceAt(neighbor))
						{
							visitedPlacements->insert(neighbor);

							// Neighboring position is a potential, verify its neighbors are empty or same color
							
							bool validPlacement = true;
							for (int dir2 = 0; dir2 < (int)Direction::NumDirections; dir2++)
							{
								auto surroundingPosition = neighbor.GetNeighborAt((Direction)dir2);
								auto surroundingPiece = GetPieceOnTopAt(surroundingPosition);
								if (surroundingPiece != PieceName::INVALID && GetColor(surroundingPiece) != m_currentColor)
								{
									validPlacement = false;
									break;
								}
							}

							if (validPlacement)
							{
								m_cachedValidPlacements->insert(neighbor);
							}
						}
					}
				}
			}
		}
	}
	return m_cachedValidPlacements;
}

bool Board::PlacingPieceInOrder(PieceName const& pieceName)
{
	if (PieceInHand(pieceName))
	{
		switch (pieceName)
		{
		case PieceName::wS2:
			return PieceInPlay(PieceName::wS1);
		case PieceName::wB2:
			return PieceInPlay(PieceName::wB1);
		case PieceName::wG2:
			return PieceInPlay(PieceName::wG1);
		case PieceName::wG3:
			return PieceInPlay(PieceName::wG2);
		case PieceName::wA2:
			return PieceInPlay(PieceName::wA1);
		case PieceName::wA3:
			return PieceInPlay(PieceName::wA2);
		case PieceName::bS2:
			return PieceInPlay(PieceName::bS1);
		case PieceName::bB2:
			return PieceInPlay(PieceName::bB1);
		case PieceName::bG2:
			return PieceInPlay(PieceName::bG1);
		case PieceName::bG3:
			return PieceInPlay(PieceName::bG2);
		case PieceName::bA2:
			return PieceInPlay(PieceName::bA1);
		case PieceName::bA3:
			return PieceInPlay(PieceName::bA2);
		}
	}

	return true;
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

PieceName Board::GetPieceOnTopAt(Position const& position)
{
	auto currentPosition = position.GetBottom();
	
	auto topPiece = GetPieceAt(currentPosition);

	if (topPiece != PieceName::INVALID)
	{
		while (true)
		{
			currentPosition = currentPosition.GetAbove();
			auto nextPiece = GetPieceAt(currentPosition);
			if (nextPiece == PieceName::INVALID)
			{
				break;
			}
			topPiece = nextPiece;
		}
	}

	return topPiece;
}

bool Board::HasPieceAt(Position const& position)
{
	return GetPieceAt(position) != PieceName::INVALID;
}

inline bool Board::PieceInHand(PieceName const& pieceName)
{
	assert(pieceName != PieceName::INVALID && pieceName != PieceName::NumPieceNames);

	return (m_piecePositions[(int)pieceName].Stack < 0);
}

inline bool Board::PieceInPlay(PieceName const& pieceName)
{
	assert(pieceName != PieceName::INVALID && pieceName != PieceName::NumPieceNames);

	return (m_piecePositions[(int)pieceName].Stack >= 0);
}

bool Board::PieceIsOnTop(PieceName const& pieceName)
{
	return PieceInPlay(pieceName) && !HasPieceAt(m_piecePositions[(int)pieceName].GetAbove());
}

void Board::ResetCaches()
{
	m_cachedValidPlacements = nullptr;
	m_cachedValidMoves = nullptr;
}
