// Copyright (c) 2020 Jon Thysell <http://jonthysell.com>
// Licensed under the MIT License.

#include <queue>
#include <sstream>
#include <assert.h>

#include "Board.h"

using namespace MzingaCpp;

#define CurrentPlayerTurn (1 + m_currentTurn / 2)

#define CurrentTurnQueenInPlay PieceInPlay(m_currentColor == Color::White ? PieceName::wQ : PieceName::bQ)

Board::Board()
{
	for (int pn = 0; pn < (int)PieceName::NumPieceNames; pn++)
	{
		m_piecePositions[pn] = Position{ 0, 0, -1 };
	}
}

BoardState Board::GetBoardState()
{
	return m_boardState;
}

int Board::GetCurrentTurn()
{
	return m_currentTurn;
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

		if (GameInProgress(m_boardState))
		{
			for (int pn = 0; pn < (int)PieceName::NumPieceNames; pn++)
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

		ResetState();
		ResetCaches();

		return true;
	}

	return false;
}

bool Board::TryUndoLastMove()
{
	if (m_moveHistory.size() > 0)
	{
		auto lastMove = m_moveHistory.back();

		if (lastMove != PassMove)
		{
			m_piecePositions[(int)lastMove.PieceName] = lastMove.Source;
		}

		m_moveHistory.pop_back();
		m_moveHistoryStr.pop_back();

		m_currentTurn--;

		ResetState();
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
			PieceName neighbor = GetPieceOnTopAt(neighborPosition);

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
					destination = targetPosition.GetNeighborAt(Direction::UpLeft).GetBottom();
					break;
				case '/':
					destination = targetPosition.GetNeighborAt(Direction::DownLeft).GetBottom();
					break;
				case '\\':
					destination = targetPosition.GetNeighborAt(Direction::Up).GetBottom();
					break;
				}
			}
			else if (afterSeperator != '\0')
			{
				// Moving piece on the right-hand side of the target piece
				switch (afterSeperator)
				{
				case '-':
					destination = targetPosition.GetNeighborAt(Direction::DownRight).GetBottom();
					break;
				case '/':
					destination = targetPosition.GetNeighborAt(Direction::UpRight).GetBottom();
					break;
				case '\\':
					destination = targetPosition.GetNeighborAt(Direction::Down).GetBottom();
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
	if (pieceName != PieceName::INVALID && GameInProgress(m_boardState) && m_currentColor == GetColor(pieceName) && PlacingPieceInOrder(pieceName))
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
		else if (CurrentTurnQueenInPlay && CanMoveWithoutBreakingHive(pieceName))
		{
			// Piece is in play and movement is allowed
			switch (GetBugType(pieceName))
			{
			case BugType::QueenBee:
				GetValidQueenBeeMoves(pieceName, moveSet);
				break;
			case BugType::Spider:
				GetValidSpiderMoves(pieceName, moveSet);
				break;
			case BugType::Beetle:
				GetValidBeetleMoves(pieceName, moveSet);
				break;
			case BugType::Grasshopper:
				GetValidGrasshopperMoves(pieceName, moveSet);
				break;
			case BugType::SoldierAnt:
				GetValidSoldierAntMoves(pieceName, moveSet);
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
		else
		{
			auto visitedPlacements = std::make_shared<PositionSet>();

			for (int pn = 0; pn < (int)PieceName::NumPieceNames; pn++)
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

void Board::GetValidQueenBeeMoves(PieceName const& pieceName, std::shared_ptr<MoveSet> moveSet)
{
	GetValidSlides(pieceName, moveSet, 1);
}

void Board::GetValidSpiderMoves(PieceName const& pieceName, std::shared_ptr<MoveSet> moveSet)
{
	GetValidSlides(pieceName, moveSet, 3);

	auto upToTwo = std::make_shared<MoveSet>();
	GetValidSlides(pieceName, upToTwo, 2);

	for (auto const& move : *upToTwo)
	{
		moveSet->erase(move);
	}
}

void Board::GetValidBeetleMoves(PieceName const& pieceName, std::shared_ptr<MoveSet> moveSet)
{
	// Look in all directions
	for (int direction = 0; direction < (int)Direction::NumDirections; direction++)
	{
		auto newPosition = m_piecePositions[(int)pieceName].GetNeighborAt((Direction)direction);

		auto topNeighbor = GetPieceOnTopAt(newPosition);

		// Get positions to left and right or direction we're heading
		auto leftOfTarget = LeftOf((Direction)direction);
		auto rightOfTarget = RightOf((Direction)direction);
		auto leftNeighborPosition = m_piecePositions[(int)pieceName].GetNeighborAt(leftOfTarget);
		auto rightNeighborPosition = m_piecePositions[(int)pieceName].GetNeighborAt(rightOfTarget);

		auto topLeftNeighbor = GetPieceOnTopAt(leftNeighborPosition);
		auto topRightNeighbor = GetPieceOnTopAt(rightNeighborPosition);

		// At least one neighbor is present
		uint32_t currentHeight = m_piecePositions[(int)pieceName].Stack + 1;
		uint32_t destinationHeight = topNeighbor != PieceName::INVALID ? m_piecePositions[(int)topNeighbor].Stack + 1 : 0;

		uint32_t topLeftNeighborHeight = topLeftNeighbor != PieceName::INVALID ? m_piecePositions[(int)topLeftNeighbor].Stack + 1 : 0;
		uint32_t topRightNeighborHeight = topRightNeighbor != PieceName::INVALID ? m_piecePositions[(int)topRightNeighbor].Stack + 1 : 0;

		// "Take-off" beetle
		currentHeight--;

		if (!(currentHeight == 0 && destinationHeight == 0 && topLeftNeighborHeight == 0 && topRightNeighborHeight == 0))
		{
			// Logic from http://boardgamegeek.com/wiki/page/Hive_FAQ#toc9
			if (!(destinationHeight < topLeftNeighborHeight && destinationHeight < topRightNeighborHeight && currentHeight < topLeftNeighborHeight && currentHeight < topRightNeighborHeight))
			{
				auto targetMove = Move{ pieceName,  m_piecePositions[(int)pieceName], Position { newPosition.Q, newPosition.R, (int)destinationHeight } };
				moveSet->insert(targetMove);
			}
		}
	}
}

void Board::GetValidGrasshopperMoves(PieceName const& pieceName, std::shared_ptr<MoveSet> moveSet)
{
	auto startingPosition = m_piecePositions[(int)pieceName];

	for (int dir = 0; dir < (int)Direction::NumDirections; dir++)
	{
		auto landingPosition = startingPosition.GetNeighborAt((Direction)dir);

		int distance = 0;
		while (HasPieceAt(landingPosition))
		{
			// Jump one more in the same direction
			landingPosition = landingPosition.GetNeighborAt((Direction)dir);
			distance++;
		}

		if (distance > 0)
		{
			// Can only move if there's at least one piece in the way
			moveSet->insert(Move{ pieceName, startingPosition, landingPosition });
		}
	}
}

void Board::GetValidSoldierAntMoves(PieceName const& pieceName, std::shared_ptr<MoveSet> moveSet)
{
	GetValidSlides(pieceName, moveSet, -1);
}

void Board::GetValidSlides(PieceName const& pieceName, std::shared_ptr<MoveSet> moveSet, int maxRange)
{
	auto startingPosition = m_piecePositions[(int)pieceName];

	auto visitedPositions = std::make_shared<PositionSet>();
	visitedPositions->insert(startingPosition);

	m_piecePositions[(int)pieceName].Stack = -1;
	GetValidSlides(pieceName, moveSet, startingPosition, startingPosition, visitedPositions, 0, maxRange);
	m_piecePositions[(int)pieceName].Stack = startingPosition.Stack;
}

void Board::GetValidSlides(PieceName const& pieceName, std::shared_ptr<MoveSet> moveSet, Position const& startingPosition, Position const& currentPosition, std::shared_ptr<PositionSet> visitedPositions, int currentRange, int maxRange)
{
	if (maxRange < 0 || currentRange < maxRange)
	{
		for (int slideDirection = 0; slideDirection < (int)Direction::NumDirections; slideDirection++)
		{
			auto slidePosition = currentPosition.GetNeighborAt((Direction)slideDirection);

			if (visitedPositions->find(slidePosition) == visitedPositions->end() && !HasPieceAt(slidePosition))
			{
				// Slide position is open

				auto left = LeftOf((Direction)slideDirection);
				auto right = RightOf((Direction)slideDirection);

				if (HasPieceAt(currentPosition.GetNeighborAt(right)) != HasPieceAt(currentPosition.GetNeighborAt(left)))
				{
					// Can slide into slide position
					auto move = Move{ pieceName, startingPosition, slidePosition };

					if (moveSet->find(move) == moveSet->end())
					{
						moveSet->insert(move);
						visitedPositions->insert(slidePosition);
						GetValidSlides(pieceName, moveSet, startingPosition, slidePosition, visitedPositions, currentRange + 1, maxRange);
					}
				}
			}
		}
	}
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
	for (int pn = 0; pn < (int)PieceName::NumPieceNames; pn++)
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

bool Board::CanMoveWithoutBreakingHive(PieceName const& pieceName)
{
	int pieceIndex = (int)pieceName;
	if (m_piecePositions[pieceIndex].Stack == 0)
	{
		// Temporarily remove piece from board
		m_piecePositions[pieceIndex].Stack = -1;

		// Determine if the hive is broken
		bool isOneHive = IsOneHive();

		// Return piece to the board
		m_piecePositions[pieceIndex].Stack = 0;

		return isOneHive;
	}
	return true;
}

bool Board::IsOneHive()
{
	bool partOfHive[(int)PieceName::NumPieceNames];
	int piecesVisited = 0;

	// Find a piece on the board to start checking
	auto startingPiece = PieceName::INVALID;
	for (int pn = 0; pn < (int)PieceName::NumPieceNames; pn++)
	{
		if (PieceInHand((PieceName)pn))
		{
			partOfHive[pn] = true;
			piecesVisited++;
		}
		else
		{
			partOfHive[pn] = false;
			if (startingPiece == PieceName::INVALID && m_piecePositions[pn].Stack == 0)
			{
				// Save off a starting piece on the bottom
				startingPiece = (PieceName)pn;
				partOfHive[pn] = true;
				piecesVisited++;
			}
		}
	}

	// There is at least one piece on the board
	if (startingPiece != PieceName::INVALID && piecesVisited < (int)PieceName::NumPieceNames)
	{
		std::queue<PieceName> piecesToLookAt;
		piecesToLookAt.push(startingPiece);

		while (piecesToLookAt.size() > 0)
		{
			auto currentPiece = piecesToLookAt.front();
			piecesToLookAt.pop();

			// Check all pieces at this stack level
			for (int dir = 0; dir < (int)Direction::NumDirections; dir++)
			{
				auto neighbor = m_piecePositions[(int)currentPiece].GetNeighborAt((Direction)dir);
				auto neighborPiece = GetPieceAt(neighbor);
				if (neighborPiece != PieceName::INVALID && !partOfHive[(int)neighborPiece])
				{
					piecesToLookAt.push(neighborPiece);
					partOfHive[(int)neighborPiece] = true;
					piecesVisited++;
				}
			}

			// Check for all pieces above this one
			auto pieceAbove = GetPieceAt(m_piecePositions[(int)currentPiece].GetAbove());
			while (PieceName::INVALID != pieceAbove)
			{
				partOfHive[(int)pieceAbove] = true;
				piecesVisited++;
				pieceAbove = GetPieceAt(m_piecePositions[(int)pieceAbove].GetAbove());
			}
		}
	}

	return piecesVisited == (int)PieceName::NumPieceNames;
}

int Board::CountNeighbors(PieceName const& pieceName)
{
	int count = 0;
	if (PieceInPlay(pieceName))
	{
		for (int dir = 0; dir < (int)Direction::NumDirections; dir++)
		{
			auto neighbor = GetPieceAt(m_piecePositions[(int)pieceName].GetNeighborAt((Direction)dir));
			if (neighbor != PieceName::INVALID)
			{
				count++;
			}
		}
	}
	return count;
}

void Board::ResetState()
{
	m_currentColor = (Color)(m_currentTurn % (int)Color::NumColors);

	bool whiteQueenSurrounded = (CountNeighbors(PieceName::wQ) == 6);
	bool blackQueenSurrounded = (CountNeighbors(PieceName::bQ) == 6);

	if (whiteQueenSurrounded && blackQueenSurrounded)
	{
		m_boardState = BoardState::Draw;
	}
	else if (whiteQueenSurrounded)
	{
		m_boardState = BoardState::BlackWins;
	}
	else if (blackQueenSurrounded)
	{
		m_boardState = BoardState::WhiteWins;
	}
	else
	{
		m_boardState = m_currentTurn == 0 ? BoardState::NotStarted : BoardState::InProgress;
	}
}

void Board::ResetCaches()
{
	m_cachedValidPlacements = nullptr;
	m_cachedValidMoves = nullptr;
}
