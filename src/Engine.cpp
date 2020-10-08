// Copyright (c) 2020 Jon Thysell <http://jonthysell.com>
// Licensed under the MIT License.

#include <sstream>

#include "Constants.h"
#include "Engine.h"

using namespace MzingaCpp;

Engine::Engine(std::function<void(std::string)> writeLine) : m_writeLine{ writeLine }
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
		NewGame();
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
	else if (line == CommandString_Options)
	{
		Options();
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

void Engine::NewGame()
{
	m_board = std::make_shared<Board>();

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

	auto validMoves = m_board->GetValidMoves();

	std::ostringstream str;
	bool first = true;
	for (auto const& iter : *validMoves)
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

void Engine::Options()
{
	WriteLine(OkString);
}

void Engine::Exit()
{
	m_exitRequested = true;
}
