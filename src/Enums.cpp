// Copyright (c) Jon Thysell <http://jonthysell.com>
// Licensed under the MIT License.

#include <cstring>

#include "Enums.h"

namespace MzingaCpp
{

std::string GetEnumString(Color const &value)
{
    switch (value)
    {
    case Color::White:
        return "White";
    case Color::Black:
        return "Black";
    default:
        return "";
    }
}

std::string GetEnumString(BoardState const &value)
{
    switch (value)
    {
    case BoardState::NotStarted:
        return "NotStarted";
    case BoardState::InProgress:
        return "InProgress";
    case BoardState::Draw:
        return "Draw";
    case BoardState::WhiteWins:
        return "WhiteWins";
    case BoardState::BlackWins:
        return "BlackWins";
    default:
        return "";
    }
}

bool GameInProgress(BoardState const &value)
{
    return (value == BoardState::NotStarted || value == BoardState::InProgress);
}

bool GameIsOver(BoardState const &value)
{
    return !GameInProgress(value);
}

std::string GetEnumString(PieceName const &value)
{
    switch (value)
    {
    case PieceName::wQ:
        return "wQ";
    case PieceName::wS1:
        return "wS1";
    case PieceName::wS2:
        return "wS2";
    case PieceName::wB1:
        return "wB1";
    case PieceName::wB2:
        return "wB2";
    case PieceName::wG1:
        return "wG1";
    case PieceName::wG2:
        return "wG2";
    case PieceName::wG3:
        return "wG3";
    case PieceName::wA1:
        return "wA1";
    case PieceName::wA2:
        return "wA2";
    case PieceName::wA3:
        return "wA3";
    case PieceName::wM:
        return "wM";
    case PieceName::wL:
        return "wL";
    case PieceName::wP:
        return "wP";
    case PieceName::bQ:
        return "bQ";
    case PieceName::bS1:
        return "bS1";
    case PieceName::bS2:
        return "bS2";
    case PieceName::bB1:
        return "bB1";
    case PieceName::bB2:
        return "bB2";
    case PieceName::bG1:
        return "bG1";
    case PieceName::bG2:
        return "bG2";
    case PieceName::bG3:
        return "bG3";
    case PieceName::bA1:
        return "bA1";
    case PieceName::bA2:
        return "bA2";
    case PieceName::bA3:
        return "bA3";
    case PieceName::bM:
        return "bM";
    case PieceName::bL:
        return "bL";
    case PieceName::bP:
        return "bP";
    default:
        return "";
    }
}

PieceName GetPieceNameValue(const char *str)
{
    if (strcmp(str, "wQ") == 0)
        return PieceName::wQ;
    else if (strcmp(str, "wS1") == 0)
        return PieceName::wS1;
    else if (strcmp(str, "wS2") == 0)
        return PieceName::wS2;
    else if (strcmp(str, "wB1") == 0)
        return PieceName::wB1;
    else if (strcmp(str, "wB2") == 0)
        return PieceName::wB2;
    else if (strcmp(str, "wG1") == 0)
        return PieceName::wG1;
    else if (strcmp(str, "wG2") == 0)
        return PieceName::wG2;
    else if (strcmp(str, "wG3") == 0)
        return PieceName::wG3;
    else if (strcmp(str, "wA1") == 0)
        return PieceName::wA1;
    else if (strcmp(str, "wA2") == 0)
        return PieceName::wA2;
    else if (strcmp(str, "wA3") == 0)
        return PieceName::wA3;
    else if (strcmp(str, "wM") == 0)
        return PieceName::wM;
    else if (strcmp(str, "wL") == 0)
        return PieceName::wL;
    else if (strcmp(str, "wP") == 0)
        return PieceName::wP;
    else if (strcmp(str, "bQ") == 0)
        return PieceName::bQ;
    else if (strcmp(str, "bS1") == 0)
        return PieceName::bS1;
    else if (strcmp(str, "bS2") == 0)
        return PieceName::bS2;
    else if (strcmp(str, "bB1") == 0)
        return PieceName::bB1;
    else if (strcmp(str, "bB2") == 0)
        return PieceName::bB2;
    else if (strcmp(str, "bG1") == 0)
        return PieceName::bG1;
    else if (strcmp(str, "bG2") == 0)
        return PieceName::bG2;
    else if (strcmp(str, "bG3") == 0)
        return PieceName::bG3;
    else if (strcmp(str, "bA1") == 0)
        return PieceName::bA1;
    else if (strcmp(str, "bA2") == 0)
        return PieceName::bA2;
    else if (strcmp(str, "bA3") == 0)
        return PieceName::bA3;
    else if (strcmp(str, "bM") == 0)
        return PieceName::bM;
    else if (strcmp(str, "bL") == 0)
        return PieceName::bL;
    else if (strcmp(str, "bP") == 0)
        return PieceName::bP;

    return PieceName::INVALID;
}

Color GetColor(PieceName const &value)
{
    switch (value)
    {
    case PieceName::wQ:
    case PieceName::wS1:
    case PieceName::wS2:
    case PieceName::wB1:
    case PieceName::wB2:
    case PieceName::wG1:
    case PieceName::wG2:
    case PieceName::wG3:
    case PieceName::wA1:
    case PieceName::wA2:
    case PieceName::wA3:
    case PieceName::wM:
    case PieceName::wL:
    case PieceName::wP:
        return Color::White;
    case PieceName::bQ:
    case PieceName::bS1:
    case PieceName::bS2:
    case PieceName::bB1:
    case PieceName::bB2:
    case PieceName::bG1:
    case PieceName::bG2:
    case PieceName::bG3:
    case PieceName::bA1:
    case PieceName::bA2:
    case PieceName::bA3:
    case PieceName::bM:
    case PieceName::bL:
    case PieceName::bP:
        return Color::Black;
    }

    return Color::NumColors;
}

Direction LeftOf(Direction const &value)
{
    return (Direction)(((int)value + (int)Direction::NumDirections - 1) % (int)Direction::NumDirections);
}

Direction RightOf(Direction const &value)
{
    return (Direction)(((int)value + 1) % (int)Direction::NumDirections);
}

BugType GetBugType(PieceName const &value)
{
    switch (value)
    {
    case PieceName::wQ:
    case PieceName::bQ:
        return BugType::QueenBee;
    case PieceName::wS1:
    case PieceName::wS2:
    case PieceName::bS1:
    case PieceName::bS2:
        return BugType::Spider;
    case PieceName::wB1:
    case PieceName::wB2:
    case PieceName::bB1:
    case PieceName::bB2:
        return BugType::Beetle;
    case PieceName::wG1:
    case PieceName::wG2:
    case PieceName::wG3:
    case PieceName::bG1:
    case PieceName::bG2:
    case PieceName::bG3:
        return BugType::Grasshopper;
    case PieceName::wA1:
    case PieceName::wA2:
    case PieceName::wA3:
    case PieceName::bA1:
    case PieceName::bA2:
    case PieceName::bA3:
        return BugType::SoldierAnt;
    case PieceName::wM:
    case PieceName::bM:
        return BugType::Mosquito;
    case PieceName::wL:
    case PieceName::bL:
        return BugType::Ladybug;
    case PieceName::wP:
    case PieceName::bP:
        return BugType::Pillbug;
    default:
        return BugType::INVALID;
    }
}

std::string GetEnumString(GameType const &value)
{
    switch (value)
    {
    case GameType::Base:
        return "Base";
    case GameType::BaseM:
        return "Base+M";
    case GameType::BaseL:
        return "Base+L";
    case GameType::BaseP:
        return "Base+P";
    case GameType::BaseML:
        return "Base+ML";
    case GameType::BaseMP:
        return "Base+MP";
    case GameType::BaseLP:
        return "Base+LP";
    case GameType::BaseMLP:
        return "Base+MLP";
    default:
        return "";
    }
}

GameType GetGameTypeValue(const char *str)
{
    if (strcmp(str, "Base") == 0)
        return GameType::Base;
    else if (strcmp(str, "Base+M") == 0)
        return GameType::BaseM;
    else if (strcmp(str, "Base+L") == 0)
        return GameType::BaseL;
    else if (strcmp(str, "Base+P") == 0)
        return GameType::BaseP;
    else if (strcmp(str, "Base+ML") == 0)
        return GameType::BaseML;
    else if (strcmp(str, "Base+MP") == 0)
        return GameType::BaseMP;
    else if (strcmp(str, "Base+LP") == 0)
        return GameType::BaseLP;
    else if (strcmp(str, "Base+MLP") == 0)
        return GameType::BaseMLP;

    return GameType::INVALID;
}

bool PieceNameIsEnabledForGameType(PieceName const &pieceName, GameType const &gameType)
{
    switch (pieceName)
    {
    case PieceName::INVALID:
        return false;
    case PieceName::wM:
    case PieceName::bM:
        return gameType == GameType::BaseM || gameType == GameType::BaseML || gameType == GameType::BaseMP ||
               gameType == GameType::BaseMLP;
    case PieceName::wL:
    case PieceName::bL:
        return gameType == GameType::BaseL || gameType == GameType::BaseML || gameType == GameType::BaseLP ||
               gameType == GameType::BaseMLP;
    case PieceName::wP:
    case PieceName::bP:
        return gameType == GameType::BaseP || gameType == GameType::BaseMP || gameType == GameType::BaseLP ||
               gameType == GameType::BaseMLP;
    default:
        return true;
    }
}

} // namespace MzingaCpp
