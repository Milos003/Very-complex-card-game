// --------------------------- CommandHandler.hpp ---------------------------
//
// This file declares the CommandHandler class, which processes all user
// input commands during the main gameplay loop.
// It handles input parsing and dispatches control to the game logic.
//
// Group: 051
//
// Author: Miloš Đukarić, Florian Kerman, Stefan Jović
// --------------------------------------------------------------------------
#pragma once

#include <string>
#include "Game.hpp"

// -------------------------------------------------------------
// CommandHandler: handles user commands during gameplay.
// -------------------------------------------------------------
class CommandHandler
{
public:
  // -------------------------------------------------------------
  //
  // Processes a single user input command. Handles validation,
  // parsing, and execution by delegating to the Game instance.
  //
  // @param input Raw user input string
  // @param game  Reference to the current Game instance
  //
  // @return true to continue the game, false to exit (e.g. "quit")
  //
  // -------------------------------------------------------------
  static bool process(const std::string &input, Game &game);

private:
  static bool handleQuit(const std::string &input, Game &game);

  static bool handleDone(const std::string &input, Game &game);

  static bool handleInfo(const std::string &input, Game &game);

  static bool handleHelp(const std::string &input, Game &game);

  static bool handleBoard(const std::string &input, Game &game);

  static bool handleStatus(const std::string &input, Game &game);

  static bool handleGraveyard(const std::string &input, Game &game);

  static bool handleCreature(const std::string &input, Game &game);

  static bool handleBattle(const std::string &input, Game &game);

  static bool handleHand(const std::string &input, Game &game);

  static bool handleRedraw(const std::string &input, Game &game);

  static bool handleSpell(const std::string &input, Game &game);

  static bool printUnknownCommand(const std::string &input, Game &game);
};
