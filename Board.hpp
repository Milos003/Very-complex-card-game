// --------------------------- Board.hpp ---------------------------
//
// This file declares the Board class, which manages four zones
// (attacker/defender field and battle) and handles board printing.
// It also provides utility methods to check field occupancy.
//
// Group: 051
//
// Author: Miloš Đukarić, Florian Kerman, Stefan Jović
// -----------------------------------------------------------------

#pragma once

#include "Zone.hpp"
#include <iostream>

class Board
{
public:
  // -------------------------------------------------------------
  //
  // Constructs a new Board with printing enabled by default and
  // initializes all four zones (field & battle for both players).
  //
  // -------------------------------------------------------------
  Board();

  // -------------------------------------------------------------
  //
  // Enables or disables automatic board printing.
  //
  // @param enabled If true, enables printing; otherwise disables.
  //
  // -------------------------------------------------------------
  void setPrinting(bool enabled);

  // -------------------------------------------------------------
  //
  // Prints the current state of the board based on the round number.
  // Adjusts which player is on top/bottom for visualization.
  //
  // @param roundNumber The round number used to determine layout.
  //
  // -------------------------------------------------------------
  void print(int roundNumber) const;

  // -------------------------------------------------------------
  //
  // Checks if a given slot in a player's field zone is occupied.
  //
  // @param playerId The player ID (1 or 2).
  // @param slot     The name of the slot (e.g., "A1", "B3").
  //
  // @return true if the slot is occupied, false otherwise.
  //
  // -------------------------------------------------------------
  bool isFieldSlotOccupied(int playerId, const std::string &slot) const;


  // -------------------------------------------------------------
  //
  // Accessors to get reference to each of the four zones.
  //
  // @return Reference to the requested zone.
  //
  // -------------------------------------------------------------
  Zone &defenderField();

  Zone &defenderBattle();

  Zone &attackerField();

  Zone &attackerBattle();

private:
  bool printing;
  Zone defField;
  Zone defBattle;
  Zone atkField;
  Zone atkBattle;
};
