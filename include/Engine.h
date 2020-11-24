// Copyright (c) 2020 Jon Thysell <http://jonthysell.com>
// Licensed under the MIT License.

#ifndef ENGINE_H
#define ENGINE_H

#include <functional>
#include <map>
#include <memory>
#include <string>

#include "Board.h"

namespace MzingaCpp
{
	class Engine
	{
	public:
		Engine(std::function<void(std::string)> writeLine);

		void Start();

		void ReadLine(std::string line);

		bool ExitRequested()
		{
			return m_exitRequested;
		}

	private:
		void WriteLine(std::string line);
		void WriteError(std::string message);
		void WriteError();

		void Info();
		void NewGame(std::string args);
		void ValidMoves();
		void BestMove();
		void Play(std::string args);
		void Pass();
		void Undo(std::string args);
		void Options();

		void Perft(std::string args);
		void Exit();

		std::function<void(std::string)> m_writeLine;
		bool m_exitRequested = false;

		std::shared_ptr<Board> m_board = nullptr;
	};
}

#endif
