// Copyright (c) Jon Thysell <http://jonthysell.com>
// Licensed under the MIT License.

#include <assert.h>
#include <queue>
#include <sstream>

#include "Board.h"

using namespace MzingaCpp;

#define CurrentPlayerTurn (1 + m_currentTurn / 2)

#define CurrentTurnQueenInPlay PieceInPlay(m_currentColor == Color::White ? PieceName::wQ : PieceName::bQ)

Board::Board(GameType gameType) : m_gameType(gameType)
{
    for (int pn = 0; pn < (int)PieceName::NumPieceNames; pn++)
    {
        m_piecePositions[pn] = NullPosition;
    }

    for (int q = 0; q < BoardSize; q++)
    {
        for (int r = 0; r < BoardSize; r++)
        {
            for (int stack = 0; stack < BoardStackSize; stack++)
            {
                m_pieceGrid[q][r][stack] = PieceName::INVALID;
            }
        }
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

    str << GetEnumString(m_gameType);
    str << ";" << GetEnumString(m_boardState);
    str << ";" << GetEnumString(m_currentColor) << "[" << CurrentPlayerTurn << "]";

    for (auto const &iter : m_moveHistoryStr)
    {
        str << ";" << iter;
    }

    return str.str();
}

std::shared_ptr<MoveSet> Board::GetValidMoves()
{
    auto validMoves = std::make_shared<MoveSet>();

    if (GameInProgress(m_boardState))
    {
        for (int pn = (int)(m_currentColor == Color::White ? PieceName::wQ : PieceName::bQ);
             pn < (int)(m_currentColor == Color::White ? PieceName::bQ : PieceName::NumPieceNames); pn++)
        {
            GetValidMoves((PieceName)pn, validMoves);
        }

        if (validMoves->size() == 0)
        {
            validMoves->insert(PassMove);
        }
    }

    return validMoves;
}

bool Board::TryPlayMove(Move const &move, std::string moveString)
{
    auto validMoves = GetValidMoves();

    auto it = validMoves->find(move);
    if (it != validMoves->end())
    {
        if (moveString.empty())
        {
            TryGetMoveString(move, moveString);
        }

        m_moveHistoryStr.push_back(moveString);

        TrustedPlay(move);

        return true;
    }

    return false;
}

bool Board::TryUndoLastMove()
{
    if (m_moveHistory.size() > 0)
    {
        auto const &lastMove = m_moveHistory.back();

        if (lastMove != PassMove)
        {
            SetPosition(lastMove.PieceName, lastMove.Source);
        }

        m_moveHistory.pop_back();
        m_moveHistoryStr.pop_back();

        m_lastPieceMoved = m_moveHistory.size() > 0 ? m_moveHistory.back().PieceName : PieceName::INVALID;

        m_currentTurn--;

        ResetState();
        ResetCaches();

        return true;
    }

    return false;
}

bool Board::TryGetMoveString(Move const &move, std::string &result)
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
        SetPosition(move.PieceName, NullPosition);
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
                case 1: // UpRight
                    endPiece = "/" + endPiece;
                    break;
                case 2: // DownRight
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
        SetPosition(move.PieceName, move.Source);
    }

    if (!endPiece.empty())
    {
        result = (startPiece + " ") + endPiece;
        return true;
    }

    return false;
}

bool Board::TryParseMove(std::string moveString, Move &result, std::string &resultString)
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

        Position source = GetPosition(startPiece);

        Position destination = OriginPosition;

        if (endPiece != PieceName::INVALID)
        {
            Position targetPosition = GetPosition(endPiece);

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

        result = Move{startPiece, source, destination};
        return true;
    }

    result = Move{};
    return false;
}

long Board::CalculatePerft(int depth)
{
    if (depth == 0)
    {
        return 1;
    }

    auto moves = GetValidMoves();

    if (depth == 1)
    {
        return moves->size();
    }

    long nodes = 0;

    for (auto const &move : *moves)
    {
        TrustedPlay(move);
        m_moveHistoryStr.push_back("");
        auto value = CalculatePerft(depth - 1);
        TryUndoLastMove();

        nodes += value;
    }

    return nodes;
}

std::shared_ptr<Board> Board::Clone()
{
    auto board = std::make_shared<Board>(m_gameType);
    for (auto const &move : m_moveHistory)
    {
        board->TrustedPlay(move);
    }
    for (auto const &moveStr : m_moveHistoryStr)
    {
        board->m_moveHistoryStr.push_back(moveStr);
    }
    return board;
}

void Board::GetValidMoves(PieceName const &pieceName, std::shared_ptr<MoveSet> moveSet)
{
    if (PieceNameIsEnabledForGameType(pieceName, m_gameType) && GameInProgress(m_boardState) &&
        m_currentColor == GetColor(pieceName) && PlacingPieceInOrder(pieceName))
    {
        if (m_currentTurn == 0)
        {
            // First turn by white
            if (pieceName != PieceName::wQ)
            {
                moveSet->insert(Move{pieceName, GetPosition(pieceName), OriginPosition});
            }
        }
        else if (m_currentTurn == 1)
        {
            // First turn by black
            if (pieceName != PieceName::bQ)
            {
                CalculateValidPlacements();
                for (auto const &iter : m_cachedValidPlacements)
                {
                    moveSet->insert(Move{pieceName, GetPosition(pieceName), iter});
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
                CalculateValidPlacements();
                for (auto const &iter : m_cachedValidPlacements)
                {
                    moveSet->insert(Move{pieceName, GetPosition(pieceName), iter});
                }
            }
        }
        else if (pieceName != m_lastPieceMoved && CurrentTurnQueenInPlay && PieceIsOnTop(pieceName))
        {
            // Piece is in play and not covered
            if (CanMoveWithoutBreakingHive(pieceName))
            {
                // Look for basic valid moves of played pieces who can move
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
                case BugType::Mosquito:
                    GetValidMosquitoMoves(pieceName, moveSet, false);
                    break;
                case BugType::Ladybug:
                    GetValidLadybugMoves(pieceName, moveSet);
                    break;
                case BugType::Pillbug:
                    GetValidPillbugBasicMoves(pieceName, moveSet);
                    GetValidPillbugSpecialMoves(pieceName, moveSet);
                    break;
                }
            }
            else
            {
                // Check for special ability moves
                switch (GetBugType(pieceName))
                {
                case BugType::Mosquito:
                    GetValidMosquitoMoves(pieceName, moveSet, true);
                    break;
                case BugType::Pillbug:
                    GetValidPillbugSpecialMoves(pieceName, moveSet);
                    break;
                }
            }
        }
    }
}

void Board::CalculateValidPlacements()
{
    if (!m_cachedValidPlacementsReady)
    {
        if (m_currentTurn == 0)
        {
            m_cachedValidPlacements.insert(OriginPosition);
        }
        else if (m_currentTurn == 1)
        {
            for (int dir = 0; dir < (int)Direction::NumDirections; dir++)
            {
                m_cachedValidPlacements.insert(OriginPosition.GetNeighborAt((Direction)dir));
            }
        }
        else
        {
            for (int pn = (int)(m_currentColor == Color::White ? PieceName::wQ : PieceName::bQ);
                 pn < (int)(m_currentColor == Color::White ? PieceName::bQ : PieceName::NumPieceNames); pn++)
            {
                auto pieceName = (PieceName)pn;

                if (PieceIsOnTop(pieceName))
                {
                    auto bottomPosition = GetPosition(pieceName).GetBottom();

                    for (int dir = 0; dir < (int)Direction::NumDirections; dir++)
                    {
                        auto neighbor = bottomPosition.GetNeighborAt((Direction)dir);

                        if (!HasPieceAt(neighbor))
                        {
                            // Neighboring position is a potential, verify its neighbors are empty or same color

                            bool validPlacement = true;
                            for (int dir2 = 0; dir2 < (int)Direction::NumDirections; dir2++)
                            {
                                auto surroundingPosition = neighbor.GetNeighborAt((Direction)dir2);
                                auto surroundingPiece = GetPieceOnTopAt(surroundingPosition);
                                if (surroundingPiece != PieceName::INVALID &&
                                    GetColor(surroundingPiece) != m_currentColor)
                                {
                                    validPlacement = false;
                                    break;
                                }
                            }

                            if (validPlacement)
                            {
                                m_cachedValidPlacements.insert(neighbor);
                            }
                        }
                    }
                }
            }
        }
        m_cachedValidPlacementsReady = true;
    }
}

void Board::GetValidQueenBeeMoves(PieceName const &pieceName, std::shared_ptr<MoveSet> moveSet)
{
    GetValidSlides(pieceName, moveSet, 1);
}

void Board::GetValidSpiderMoves(PieceName const &pieceName, std::shared_ptr<MoveSet> moveSet)
{
    GetValidSlides(pieceName, moveSet, 3);
}

void Board::GetValidBeetleMoves(PieceName const &pieceName, std::shared_ptr<MoveSet> moveSet)
{
    auto position = GetPosition(pieceName);

    // Look in all directions
    for (int direction = 0; direction < (int)Direction::NumDirections; direction++)
    {
        auto newPosition = position.GetNeighborAt((Direction)direction);

        auto topNeighbor = GetPieceOnTopAt(newPosition);

        // Get positions to left and right or direction we're heading
        auto leftOfTarget = LeftOf((Direction)direction);
        auto rightOfTarget = RightOf((Direction)direction);
        auto leftNeighborPosition = position.GetNeighborAt(leftOfTarget);
        auto rightNeighborPosition = position.GetNeighborAt(rightOfTarget);

        auto topLeftNeighbor = GetPieceOnTopAt(leftNeighborPosition);
        auto topRightNeighbor = GetPieceOnTopAt(rightNeighborPosition);

        // At least one neighbor is present
        uint32_t currentHeight = position.Stack + 1;
        uint32_t destinationHeight = topNeighbor != PieceName::INVALID ? GetPosition(topNeighbor).Stack + 1 : 0;

        uint32_t topLeftNeighborHeight =
            topLeftNeighbor != PieceName::INVALID ? GetPosition(topLeftNeighbor).Stack + 1 : 0;
        uint32_t topRightNeighborHeight =
            topRightNeighbor != PieceName::INVALID ? GetPosition(topRightNeighbor).Stack + 1 : 0;

        // "Take-off" beetle
        currentHeight--;

        if (!(currentHeight == 0 && destinationHeight == 0 && topLeftNeighborHeight == 0 &&
              topRightNeighborHeight == 0))
        {
            // Logic from http://boardgamegeek.com/wiki/page/Hive_FAQ#toc9
            if (!(destinationHeight < topLeftNeighborHeight && destinationHeight < topRightNeighborHeight &&
                  currentHeight < topLeftNeighborHeight && currentHeight < topRightNeighborHeight))
            {
                auto targetMove =
                    Move{pieceName, position, Position{newPosition.Q, newPosition.R, (int)destinationHeight}};
                moveSet->insert(targetMove);
            }
        }
    }
}

void Board::GetValidGrasshopperMoves(PieceName const &pieceName, std::shared_ptr<MoveSet> moveSet)
{
    auto startingPosition = GetPosition(pieceName);

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
            moveSet->insert(Move{pieceName, startingPosition, landingPosition});
        }
    }
}

void Board::GetValidSoldierAntMoves(PieceName const &pieceName, std::shared_ptr<MoveSet> moveSet)
{
    GetValidSlides(pieceName, moveSet, 0);
}

void Board::GetValidMosquitoMoves(PieceName const &pieceName, std::shared_ptr<MoveSet> moveSet,
                                  bool const &specialAbilityOnly)
{
    auto position = GetPosition(pieceName);

    if (position.Stack > 0 && !specialAbilityOnly)
    {
        // Mosquito on top acts like a beetle
        GetValidBeetleMoves(pieceName, moveSet);
        return;
    }

    bool bugTypesEvaluated[(int)BugType::NumBugTypes] = {};

    for (int dir = 0; dir < (int)Direction::NumDirections; dir++)
    {
        auto neighborPosition = position.GetNeighborAt((Direction)dir);
        auto neighborPieceName = GetPieceOnTopAt(neighborPosition);

        auto neighborBugType = GetBugType(neighborPieceName);

        if (neighborPieceName != PieceName::INVALID && !bugTypesEvaluated[(int)(neighborBugType)])
        {
            auto newMoves = std::make_shared<MoveSet>();
            if (specialAbilityOnly)
            {
                if (neighborBugType == BugType::Pillbug)
                {
                    GetValidPillbugSpecialMoves(pieceName, newMoves);
                }
            }
            else
            {
                switch (neighborBugType)
                {
                case BugType::QueenBee:
                    GetValidQueenBeeMoves(pieceName, newMoves);
                    break;
                case BugType::Spider:
                    GetValidSpiderMoves(pieceName, newMoves);
                    break;
                case BugType::Beetle:
                    GetValidBeetleMoves(pieceName, newMoves);
                    break;
                case BugType::Grasshopper:
                    GetValidGrasshopperMoves(pieceName, newMoves);
                    break;
                case BugType::SoldierAnt:
                    GetValidSoldierAntMoves(pieceName, newMoves);
                    break;
                case BugType::Ladybug:
                    GetValidLadybugMoves(pieceName, newMoves);
                    break;
                case BugType::Pillbug:
                    GetValidPillbugBasicMoves(pieceName, newMoves);
                    GetValidPillbugSpecialMoves(pieceName, newMoves);
                    break;
                }
            }

            if (!newMoves->empty())
            {
                for (auto const &it : *newMoves)
                {
                    moveSet->insert(it);
                }
            }

            bugTypesEvaluated[(int)(neighborBugType)] = true;
        }
    }
}

void Board::GetValidLadybugMoves(PieceName const &pieceName, std::shared_ptr<MoveSet> moveSet)
{
    auto startingPosition = GetPosition(pieceName);

    auto firstMoves = std::make_shared<MoveSet>();
    GetValidBeetleMoves(pieceName, firstMoves);

    for (auto const &firstMove : *firstMoves)
    {
        if (firstMove.Destination.Stack > 0)
        {
            SetPosition(pieceName, firstMove.Destination);

            auto secondMoves = std::make_shared<MoveSet>();
            GetValidBeetleMoves(pieceName, secondMoves);

            for (auto const &secondMove : *secondMoves)
            {
                if (secondMove.Destination.Stack > 0)
                {
                    SetPosition(pieceName, secondMove.Destination);

                    auto thirdMoves = std::make_shared<MoveSet>();
                    GetValidBeetleMoves(pieceName, thirdMoves);

                    for (auto const &thirdMove : *thirdMoves)
                    {
                        if (thirdMove.Destination.Stack == 0 && thirdMove.Destination != startingPosition)
                        {
                            auto finalMove = Move{pieceName, startingPosition, thirdMove.Destination};
                            moveSet->insert(finalMove);
                        }
                    }

                    SetPosition(pieceName, firstMove.Destination);
                }
            }

            SetPosition(pieceName, startingPosition);
        }
    }
}

void Board::GetValidPillbugBasicMoves(PieceName const &pieceName, std::shared_ptr<MoveSet> moveSet)
{
    GetValidSlides(pieceName, moveSet, 1);
}

void Board::GetValidPillbugSpecialMoves(PieceName const &pieceName, std::shared_ptr<MoveSet> moveSet)
{
    auto position = GetPosition(pieceName);
    auto positionAboveTargetPiece = position.GetAbove();

    for (int dir = 0; dir < (int)Direction::NumDirections; dir++)
    {
        auto neighborPosition = position.GetNeighborAt((Direction)dir);
        auto neighborPieceName = GetPieceAt(neighborPosition);

        if (neighborPieceName != PieceName::INVALID && neighborPieceName != m_lastPieceMoved &&
            !HasPieceAt(neighborPosition, Direction::Above) && CanMoveWithoutBreakingHive(neighborPieceName))
        {
            // Piece can be moved
            auto firstMove = Move{neighborPieceName, neighborPosition, positionAboveTargetPiece};
            auto firstMoves = std::make_shared<MoveSet>();
            GetValidBeetleMoves(neighborPieceName, firstMoves);
            if (firstMoves->find(firstMove) != firstMoves->end())
            {
                // Piece can be moved on top
                SetPosition(neighborPieceName, positionAboveTargetPiece);

                auto secondMoves = std::make_shared<MoveSet>();
                GetValidBeetleMoves(neighborPieceName, secondMoves);

                for (auto const &secondMove : *secondMoves)
                {
                    if (secondMove.Destination.Stack == 0 && secondMove.Destination != neighborPosition)
                    {
                        auto finalMove = Move{neighborPieceName, neighborPosition, secondMove.Destination};
                        moveSet->insert(finalMove);
                    }
                }

                SetPosition(neighborPieceName, neighborPosition);
            }
        }
    }
}

void Board::GetValidSlides(PieceName const &pieceName, std::shared_ptr<MoveSet> moveSet, int fixedRange)
{
    auto startingPosition = GetPosition(pieceName);
    SetPosition(pieceName, NullPosition);

    if (fixedRange > 0)
    {
        GetValidSlides(pieceName, moveSet, startingPosition, startingPosition, startingPosition, fixedRange);
    }
    else
    {
        GetValidSlides(pieceName, moveSet, startingPosition, startingPosition, startingPosition);
    }

    SetPosition(pieceName, startingPosition);
}

void Board::GetValidSlides(PieceName const &pieceName, std::shared_ptr<MoveSet> moveSet,
                           Position const &startingPosition, Position const &lastPosition,
                           Position const &currentPosition)
{
    for (int slideDirection = 0; slideDirection < (int)Direction::NumDirections; slideDirection++)
    {
        auto const &slidePosition = currentPosition.GetNeighborAt((Direction)slideDirection);
        if (slidePosition != lastPosition && slidePosition != startingPosition && !HasPieceAt(slidePosition))
        {
            // Slide position is open
            if (HasPieceAt(currentPosition.GetNeighborAt(RightOf((Direction)slideDirection))) !=
                HasPieceAt(currentPosition.GetNeighborAt(LeftOf((Direction)slideDirection))))
            {
                // Can slide into slide position
                auto move = Move{pieceName, startingPosition, slidePosition};

                if (moveSet->find(move) == moveSet->end())
                {
                    moveSet->insert(move);
                    GetValidSlides(pieceName, moveSet, startingPosition, currentPosition, slidePosition);
                }
            }
        }
    }
}

void Board::GetValidSlides(PieceName const &pieceName, std::shared_ptr<MoveSet> moveSet,
                           Position const &startingPosition, Position const &lastPosition,
                           Position const &currentPosition, int remainingSlides)
{
    if (remainingSlides == 0)
    {
        auto move = Move{pieceName, startingPosition, currentPosition};
        if (moveSet->find(move) == moveSet->end())
        {
            moveSet->insert(move);
        }
    }
    else
    {
        for (int slideDirection = 0; slideDirection < (int)Direction::NumDirections; slideDirection++)
        {
            auto const &slidePosition = currentPosition.GetNeighborAt((Direction)slideDirection);
            if (slidePosition != lastPosition && slidePosition != startingPosition && !HasPieceAt(slidePosition))
            {
                // Slide position is open
                if (HasPieceAt(currentPosition.GetNeighborAt(RightOf((Direction)slideDirection))) !=
                    HasPieceAt(currentPosition.GetNeighborAt(LeftOf((Direction)slideDirection))))
                {
                    // Can slide into slide position
                    GetValidSlides(pieceName, moveSet, startingPosition, currentPosition, slidePosition,
                                   remainingSlides - 1);
                }
            }
        }
    }
}

void Board::TrustedPlay(Move const &move)
{
    m_moveHistory.push_back(move);

    if (move != PassMove)
    {
        SetPosition(move.PieceName, move.Destination);
    }

    m_currentTurn++;
    m_lastPieceMoved = move.PieceName;

    ResetState();
    ResetCaches();
}

bool Board::PlacingPieceInOrder(PieceName const &pieceName)
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

inline Position Board::GetPosition(PieceName const &pieceName)
{
    return m_piecePositions[(int)pieceName];
}

inline void Board::SetPosition(PieceName const &pieceName, Position const &position)
{
    auto oldPosition = GetPosition(pieceName);
    m_piecePositions[(int)pieceName] = position;

    if (oldPosition.Stack >= 0)
    {
        m_pieceGrid[(BoardSize / 2) + oldPosition.Q][(BoardSize / 2) + oldPosition.R][oldPosition.Stack] =
            PieceName::INVALID;
    }
    if (position.Stack >= 0)
    {
        m_pieceGrid[(BoardSize / 2) + position.Q][(BoardSize / 2) + position.R][position.Stack] = pieceName;
    }
}

inline PieceName Board::GetPieceAt(Position const &position)
{
    assert(position.Stack >= 0);
    return m_pieceGrid[(BoardSize / 2) + position.Q][(BoardSize / 2) + position.R][position.Stack];
}

PieceName Board::GetPieceAt(Position const &position, Direction const &direction)
{
    assert(position.Stack >= 0);
    return m_pieceGrid[(BoardSize / 2) + position.Q + NeighborDeltas[(int)direction][0]]
                      [(BoardSize / 2) + position.R + NeighborDeltas[(int)direction][1]]
                      [position.Stack + NeighborDeltas[(int)direction][2]];
}

PieceName Board::GetPieceOnTopAt(Position const &position)
{
    auto topPieceName = PieceName::INVALID;
    for (int stack = 0; stack < BoardStackSize; stack++)
    {
        auto pieceName = m_pieceGrid[(BoardSize / 2) + position.Q][(BoardSize / 2) + position.R][stack];
        if (pieceName == PieceName::INVALID)
        {
            break;
        }
        topPieceName = pieceName;
    }

    return topPieceName;
}

bool Board::HasPieceAt(Position const &position)
{
    return GetPieceAt(position) != PieceName::INVALID;
}

bool Board::HasPieceAt(Position const &position, Direction const &direction)
{
    return GetPieceAt(position, direction) != PieceName::INVALID;
}

inline bool Board::PieceInHand(PieceName const &pieceName)
{
    assert(pieceName != PieceName::INVALID && pieceName != PieceName::NumPieceNames);

    return (GetPosition(pieceName).Stack < 0);
}

inline bool Board::PieceInPlay(PieceName const &pieceName)
{
    assert(pieceName != PieceName::INVALID && pieceName != PieceName::NumPieceNames);

    return (GetPosition(pieceName).Stack >= 0);
}

bool Board::PieceIsOnTop(PieceName const &pieceName)
{
    return PieceInPlay(pieceName) && !HasPieceAt(GetPosition(pieceName), Direction::Above);
}

bool Board::CanMoveWithoutBreakingHive(PieceName const &pieceName)
{
    auto position = GetPosition(pieceName);
    if (position.Stack == 0)
    {
        // Temporarily remove piece from board
        SetPosition(pieceName, NullPosition);

        // Determine if the hive is broken
        bool isOneHive = IsOneHive();

        // Return piece to the board
        SetPosition(pieceName, position);

        return isOneHive;
    }
    return true;
}

bool Board::IsOneHive()
{
    bool partOfHive[(int)PieceName::NumPieceNames] = {};
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
            if (startingPiece == PieceName::INVALID && GetPosition((PieceName)pn).Stack == 0)
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

            auto currentPosition = GetPosition(currentPiece);

            // Check all pieces at this stack level
            for (int dir = 0; dir < (int)Direction::NumDirections; dir++)
            {
                auto neighborPiece = GetPieceAt(currentPosition, (Direction)dir);
                if (neighborPiece != PieceName::INVALID && !partOfHive[(int)neighborPiece])
                {
                    piecesToLookAt.push(neighborPiece);
                    partOfHive[(int)neighborPiece] = true;
                    piecesVisited++;
                }
            }

            // Check for all pieces above this one
            auto pieceAbove = GetPieceAt(currentPosition, Direction::Above);
            while (PieceName::INVALID != pieceAbove)
            {
                partOfHive[(int)pieceAbove] = true;
                piecesVisited++;
                pieceAbove = GetPieceAt(GetPosition(pieceAbove), Direction::Above);
            }
        }
    }

    return piecesVisited == (int)PieceName::NumPieceNames;
}

int Board::CountNeighbors(PieceName const &pieceName)
{
    int count = 0;
    auto position = GetPosition(pieceName);
    if (position.Stack >= 0)
    {
        for (int dir = 0; dir < (int)Direction::NumDirections; dir++)
        {
            auto neighbor = GetPieceAt(position, (Direction)dir);
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
    m_cachedValidPlacementsReady = false;
    m_cachedValidPlacements.clear();
}
