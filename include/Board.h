// Copyright (c) 2020 Jon Thysell <http://jonthysell.com>
// Licensed under the MIT License.

#ifndef BOARD_H
#define BOARD_H

#include <string>

#include "Constants.h"
#include "Enums.h"
#include "Move.h"
#include "MoveSet.h"
#include "Position.h"
#include "PositionSet.h"

namespace MzingaCpp
{
	class Board
	{
	public:
		Board();

		std::string GetGameString();
		std::shared_ptr<MoveSet> GetValidMoves();

		bool TryPlayMove(Move const& move, std::string moveString);

		bool TryGetMoveString(Move const& move, std::string& result);
		bool TryParseMove(std::string moveString, Move& result, std::string& resultString);

	private:
		void GetValidMoves(PieceName const& pieceName, std::shared_ptr<MoveSet> moveSet);
		std::shared_ptr<PositionSet> GetValidPlacements();

		PieceName GetPieceAt(Position const& position);

		void ResetCaches();

		BoardState m_boardState = BoardState::NotStarted;
		Color m_currentColor = Color::White;
		int m_currentTurn = 0;

		Position m_piecePositions[(int)PieceName::NumPieceNames];

		std::vector<Move> m_moveHistory;
		std::vector<std::string> m_moveHistoryStr;

		std::shared_ptr<MoveSet> m_cachedValidMoves = nullptr;
		std::shared_ptr<PositionSet> m_cachedValidPlacements = nullptr;
	};
}

#endif
