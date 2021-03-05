// Copyright (c) Jon Thysell <http://jonthysell.com>
// Licensed under the MIT License.

#ifndef BOARD_H
#define BOARD_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

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
    Board(GameType gameType);

    BoardState GetBoardState();
    int GetCurrentTurn();

    std::string GetGameString();
    std::shared_ptr<MoveSet> GetValidMoves();

    bool TryPlayMove(Move const &move, std::string moveString);
    bool TryUndoLastMove();

    bool TryGetMoveString(Move const &move, std::string &result);
    bool TryParseMove(std::string moveString, Move &result, std::string &resultString);

    long CalculatePerft(int depth);

    std::shared_ptr<Board> Clone();

  private:
    void GetValidMoves(PieceName const &pieceName, std::shared_ptr<MoveSet> moveSet);
    void CalculateValidPlacements();

    void GetValidQueenBeeMoves(PieceName const &pieceName, std::shared_ptr<MoveSet> moveSet);
    void GetValidSpiderMoves(PieceName const &pieceName, std::shared_ptr<MoveSet> moveSet);
    void GetValidBeetleMoves(PieceName const &pieceName, std::shared_ptr<MoveSet> moveSet);
    void GetValidGrasshopperMoves(PieceName const &pieceName, std::shared_ptr<MoveSet> moveSet);
    void GetValidSoldierAntMoves(PieceName const &pieceName, std::shared_ptr<MoveSet> moveSet);
    void GetValidMosquitoMoves(PieceName const &pieceName, std::shared_ptr<MoveSet> moveSet,
                               bool const &specialAbilityOnly);
    void GetValidLadybugMoves(PieceName const &pieceName, std::shared_ptr<MoveSet> moveSet);
    void GetValidPillbugBasicMoves(PieceName const &pieceName, std::shared_ptr<MoveSet> moveSet);
    void GetValidPillbugSpecialMoves(PieceName const &pieceName, std::shared_ptr<MoveSet> moveSet);

    void GetValidSlides(PieceName const &pieceName, std::shared_ptr<MoveSet> moveSet, int maxRange);
    void GetValidSlides(PieceName const &pieceName, std::shared_ptr<MoveSet> moveSet, Position const &startingPosition,
                        Position const &currentPosition, std::shared_ptr<PositionSet> visitedPositions,
                        int currentRange, int maxRange);

    bool CanSlideToPositionInExactRange(PieceName const &pieceName, Position const &targetPosition, int targetRange);
    bool CanSlideToPositionInExactRange(PieceName const &pieceName, Position const &targetPosition,
                                        Position const &lastPosition, Position const &currentPosition, int currentRange,
                                        int targetRange);

    void TrustedPlay(Move const &move);

    bool PlacingPieceInOrder(PieceName const &pieceName);

    Position GetPosition(PieceName const &pieceName);
    void SetPosition(PieceName const &pieceName, Position const& position);

    PieceName GetPieceAt(Position const &position);
    PieceName GetPieceOnTopAt(Position const &position);
    bool HasPieceAt(Position const &position);

    bool PieceInHand(PieceName const &pieceName);
    bool PieceInPlay(PieceName const &pieceName);

    bool PieceIsOnTop(PieceName const &pieceName);

    bool CanMoveWithoutBreakingHive(PieceName const &pieceName);

    bool IsOneHive();

    int CountNeighbors(PieceName const &pieceName);

    void ResetState();
    void ResetCaches();

    GameType m_gameType = GameType::Base;
    BoardState m_boardState = BoardState::NotStarted;
    Color m_currentColor = Color::White;
    int m_currentTurn = 0;

    PieceName m_lastPieceMoved = PieceName::INVALID;

    Position m_piecePositions[(int)PieceName::NumPieceNames];
    std::unordered_map<Position,PieceName,PositionHasher> m_piecePositionMap;

    std::vector<Move> m_moveHistory;
    std::vector<std::string> m_moveHistoryStr;

    PositionSet m_cachedValidPlacements;
    bool m_cachedValidPlacementsReady = false;
};
} // namespace MzingaCpp

#endif
