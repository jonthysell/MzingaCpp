// Copyright (c) Jon Thysell <http://jonthysell.com>
// Licensed under the MIT License.

#ifndef ENUMS_H
#define ENUMS_H

#include <string>

namespace MzingaCpp
{
enum class Color
{
    White = 0,
    Black,
    NumColors,
};

std::string GetEnumString(Color const &value);

enum class BoardState
{
    NotStarted = 0,
    InProgress,
    Draw,
    WhiteWins,
    BlackWins,
};

std::string GetEnumString(BoardState const &value);

bool GameInProgress(BoardState const &value);
bool GameIsOver(BoardState const &value);

enum class PieceName
{
    INVALID = -1,
    wQ = 0,
    wS1,
    wS2,
    wB1,
    wB2,
    wG1,
    wG2,
    wG3,
    wA1,
    wA2,
    wA3,
    wM,
    wL,
    wP,
    bQ,
    bS1,
    bS2,
    bB1,
    bB2,
    bG1,
    bG2,
    bG3,
    bA1,
    bA2,
    bA3,
    bM,
    bL,
    bP,
    NumPieceNames
};

std::string GetEnumString(PieceName const &value);
PieceName GetPieceNameValue(const char *str);

Color GetColor(PieceName const &value);

enum class Direction
{
    Up = 0,
    UpRight,
    DownRight,
    Down,
    DownLeft,
    UpLeft,
    NumDirections,
};

Direction LeftOf(Direction value);
Direction RightOf(Direction value);

enum class BugType
{
    INVALID = -1,
    QueenBee = 0,
    Spider,
    Beetle,
    Grasshopper,
    SoldierAnt,
    Mosquito,
    Ladybug,
    Pillbug,
    NumBugTypes,
};

BugType GetBugType(PieceName const &value);

enum class GameType
{
    INVALID = -1,
    Base = 0,
    BaseM,
    BaseL,
    BaseP,
    BaseML,
    BaseMP,
    BaseLP,
    BaseMLP,
};

std::string GetEnumString(GameType const &value);
GameType GetGameTypeValue(const char *str);

bool PieceNameIsEnabledForGameType(PieceName const &pieceName, GameType const &gameType);

} // namespace MzingaCpp

#endif
