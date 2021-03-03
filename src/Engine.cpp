// Copyright (c) Jon Thysell <http://jonthysell.com>
// Licensed under the MIT License.

#include <chrono>
#include <sstream>

#include "Constants.h"
#include "Engine.h"

using namespace MzingaCpp;

Engine::Engine(std::function<void(std::string)> writeLine) : m_writeLine{writeLine}
{
}

void Engine::Start()
{
    Info();
}

void Engine::ReadLine(std::string line)
{
    std::string command;
    std::string args;

    std::istringstream ss(line);
    std::ostringstream argStream;

    std::string token;

    int itemIndex = 0;
    while (std::getline(ss, token, ' '))
    {
        if (!token.empty())
        {
            if (itemIndex == 0)
            {
                command = token;
            }
            else if (itemIndex == 1)
            {
                argStream << token;
            }
            else
            {
                argStream << " " << token;
            }
            itemIndex++;
        }
    }

    args = argStream.str();

    if (command == CommandString_Info)
    {
        Info();
    }
    else if (command == CommandString_NewGame)
    {
        NewGame(args);
    }
    else if (command == CommandString_ValidMoves)
    {
        ValidMoves();
    }
    else if (command == CommandString_BestMove)
    {
        BestMove();
    }
    else if (command == CommandString_Play)
    {
        Play(args);
    }
    else if (command == CommandString_Pass)
    {
        Pass();
    }
    else if (command == CommandString_Undo)
    {
        Undo(args);
    }
    else if (command == CommandString_Options)
    {
        Options();
    }
    else if (command == CommandString_Perft)
    {
        Perft(args);
    }
    else if (command == CommandString_Exit)
    {
        Exit();
    }
    else
    {
        WriteError(ErrorMessage_InvalidCommand);
    }
}

void Engine::WriteLine(std::string line)
{
    m_writeLine(line);
}

void Engine::WriteError(std::string message)
{
    WriteLine(ErrString + (" " + message));
    WriteLine(OkString);
}

void Engine::WriteError()
{
    WriteError(ErrorMessage_Unknown);
}

void Engine::Info()
{
    WriteLine(IdString);
    WriteLine(CapabilitiesString);
    WriteLine(OkString);
}

void Engine::NewGame(std::string args)
{
    GameType gameType = GameType::Base;

    if (!args.empty())
    {
        std::istringstream ss(args);

        std::string token;

        int itemIndex = 0;
        while (std::getline(ss, token, ';'))
        {
            if (!token.empty())
            {
                if (itemIndex == 0)
                {
                    gameType = GetGameTypeValue(token.c_str());
                    if (gameType == GameType::INVALID)
                    {
                        WriteError(ErrorMessage_Unknown);
                        return;
                    }
                    m_board = std::make_shared<Board>(gameType);
                }
                if (itemIndex > 2)
                {
                    Move move;
                    std::string parsedMoveString;
                    if (!m_board->TryParseMove(token, move, parsedMoveString) ||
                        !m_board->TryPlayMove(move, parsedMoveString))
                    {
                        WriteError(ErrorMessage_Unknown);
                        return;
                    }
                }

                itemIndex++;
            }
        }
    }

    if (m_board == nullptr)
    {
        m_board = std::make_shared<Board>(gameType);
    }

    WriteLine(m_board->GetGameString());
    WriteLine(OkString);
}

void Engine::ValidMoves()
{
    if (!m_board)
    {
        WriteError(ErrorMessage_NoGameInProgress);
        return;
    }

    if (GameIsOver(m_board->GetBoardState()))
    {
        WriteError(ErrorMessage_GameIsOver);
        return;
    }

    auto validMoves = m_board->GetValidMoves();

    std::ostringstream str;
    bool first = true;
    for (auto const &iter : *validMoves)
    {
        std::string moveString;
        if (m_board->TryGetMoveString(iter, moveString))
        {
            if (!first)
            {
                str << ";";
            }
            str << moveString;
        }
        first = false;
    }

    WriteLine(str.str());
    WriteLine(OkString);
}

void Engine::BestMove()
{
    if (!m_board)
    {
        WriteError(ErrorMessage_NoGameInProgress);
        return;
    }

    if (GameIsOver(m_board->GetBoardState()))
    {
        WriteError(ErrorMessage_GameIsOver);
        return;
    }

    auto validMoves = m_board->GetValidMoves();

    Move bestMove = *(validMoves->begin());

    std::string result;
    if (m_board->TryGetMoveString(bestMove, result))
    {
        WriteLine(result);
        WriteLine(OkString);
    }
    else
    {
        WriteError();
    }
}

void Engine::Play(std::string args)
{
    if (!m_board)
    {
        WriteError(ErrorMessage_NoGameInProgress);
        return;
    }

    if (GameIsOver(m_board->GetBoardState()))
    {
        WriteError(ErrorMessage_GameIsOver);
        return;
    }

    Move move;
    std::string moveString;
    if (m_board->TryParseMove(args, move, moveString) && m_board->TryPlayMove(move, moveString))
    {
        WriteLine(m_board->GetGameString());
    }
    else
    {
        std::ostringstream invalidMove;
        invalidMove << InvalidMoveString << " " << InvalidMoveMessage_Generic;
        WriteLine(invalidMove.str());
    }

    WriteLine(OkString);
}

void Engine::Pass()
{
    Play(PassMoveString);
}

void Engine::Undo(std::string args)
{
    if (!m_board)
    {
        WriteError(ErrorMessage_NoGameInProgress);
        return;
    }

    std::istringstream ss(args);
    int movesToUndo;
    if ((ss >> movesToUndo).fail())
    {
        movesToUndo = 1;
    }

    if (movesToUndo < 1 || movesToUndo > m_board->GetCurrentTurn())
    {
        WriteError(ErrorMessage_UnableToUndo);
        return;
    }

    for (int i = 0; i < movesToUndo; i++)
    {
        m_board->TryUndoLastMove();
    }

    WriteLine(m_board->GetGameString());
    WriteLine(OkString);
}

void Engine::Options()
{
    WriteLine(OkString);
}

void Engine::Perft(std::string args)
{
    if (!m_board)
    {
        WriteError(ErrorMessage_NoGameInProgress);
        return;
    }

    std::istringstream ss(args);
    int maxDepth;
    if ((ss >> maxDepth).fail())
    {
        maxDepth = 0;
    }

    if (maxDepth < 0)
    {
        WriteError(ErrorMessage_Unknown);
        return;
    }

    for (int depth = 0; depth <= maxDepth; depth++)
    {
        auto startTime = std::chrono::high_resolution_clock::now();
        auto nodes = m_board->CalculatePerft(depth);
        auto endTime = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        std::ostringstream out;
        out << "perft(" << depth << ") = " << nodes << " in " << duration.count() << " ms. "
            << round(nodes / (double)duration.count()) << " KN/s";
        WriteLine(out.str());
    }

    WriteLine(OkString);
}

void Engine::Exit()
{
    m_exitRequested = true;
}
