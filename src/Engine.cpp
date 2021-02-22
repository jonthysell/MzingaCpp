// Copyright (c) 2020 Jon Thysell <http://jonthysell.com>
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
    auto space = line.find(" ", 0);
    auto args = space >= 0 ? line.substr(space + 1) : "";

    if (line == CommandString_Info)
    {
        Info();
    }
    else if (line.rfind(CommandString_NewGame, 0) == 0)
    {
        NewGame(args);
    }
    else if (line == CommandString_ValidMoves)
    {
        ValidMoves();
    }
    else if (line.rfind(CommandString_BestMove, 0) == 0)
    {
        BestMove();
    }
    else if (line.rfind(CommandString_Play, 0) == 0)
    {
        Play(args);
    }
    else if (line == CommandString_Pass)
    {
        Pass();
    }
    else if (line.rfind(CommandString_Undo, 0) == 0)
    {
        Undo(args);
    }
    else if (line == CommandString_Options)
    {
        Options();
    }
    else if (line.rfind(CommandString_Perft, 0) == 0)
    {
        Perft(args);
    }
    else if (line == CommandString_Exit)
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
    WriteLine(OkString);
}

void Engine::NewGame(std::string args)
{
    m_board = std::make_shared<Board>();

    if (!args.empty())
    {
        std::istringstream ss(args);

        std::string delimiter = ";";
        std::string token;
        std::string::iterator it;

        int itemIndex = 0;
        while (std::getline(ss, token, *(it = delimiter.begin())))
        {
            while (*(++it))
            {
                ss.get();
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
        auto nodes = m_board->ParallelPerft(depth);
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
