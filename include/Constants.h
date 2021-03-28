// Copyright (c) Jon Thysell <http://jonthysell.com>
// Licensed under the MIT License.

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

namespace MzingaCpp
{
constexpr const char *IdString = "id MzingaCpp 0.9.2";
constexpr const char *CapabilitiesString = "Mosquito;Ladybug;Pillbug";

constexpr const char *OkString = "ok";

constexpr const char *CommandString_Info = "info";
constexpr const char *CommandString_NewGame = "newgame";
constexpr const char *CommandString_ValidMoves = "validmoves";
constexpr const char *CommandString_BestMove = "bestmove";
constexpr const char *CommandString_Play = "play";
constexpr const char *CommandString_Pass = "pass";
constexpr const char *CommandString_Undo = "undo";
constexpr const char *CommandString_Options = "options";

constexpr const char *CommandString_Perft = "perft";
constexpr const char *CommandString_Exit = "exit";

constexpr const char *ErrString = "err";
constexpr const char *ErrorMessage_InvalidCommand = "Invalid command.";
constexpr const char *ErrorMessage_NoGameInProgress = "No game in progress. Try 'newgame' to start a new game.";
constexpr const char *ErrorMessage_GameIsOver = "The game is over. Try 'newgame' to start a new game.";
constexpr const char *ErrorMessage_UnableToUndo = "Unable to undo that many moves.";
constexpr const char *ErrorMessage_Unknown = "An unknown error has occured.";

constexpr const char *InvalidMoveString = "invalidmove";
constexpr const char *InvalidMoveMessage_Generic = "Unable to play that move at this time.";

constexpr const char *PassMoveString = "pass";

constexpr const int BoardSize = 128;
constexpr const int BoardStackSize = 6;

} // namespace MzingaCpp

#endif
