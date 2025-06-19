//---------------------------------------------------------------------------------------------------------------------
//
// This file implements the Board class, which handles printing the board,
// accessing specific zones, and checking slot occupancy. It provides
// visual representation of the field and battle zones for both players.
//
// Group: 051
//
// Author: Miloš Đukarić, Florian Kerman, Stefan Jović
//--------------------------------------------------------------------------------------------------------------------

#include "Board.hpp"
#include <iostream>
using namespace std;

//---------------------------------------------------------------------------------------------------------------------
///
/// Constructor for Board. Initializes the printing flag and sets
/// default zone identifiers for both attacker and defender.
///
//---------------------------------------------------------------------------------------------------------------------
Board::Board()
  : printing(true) // board printing enabled by default
    , defField('F') // Field-zone marker for defender
    , defBattle('B') // Battle-zone marker for defender
    , atkField('F') // Field-zone marker for attacker
    , atkBattle('B') // Battle-zone marker for attacker
{
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Enables or disables automatic printing of the board during the game.
///
/// @param enabled If true, board will be printed; otherwise not.
//---------------------------------------------------------------------------------------------------------------------
void Board::setPrinting(bool enabled)
{
  // Enable or disable automatic board redraws
  printing = enabled;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Prints the current state of the game board based on the given round number.
/// Displays both players' field and battle zones with proper orientation,
/// depending on who is the attacker.
///
/// @param roundNumber The current round number used to determine player orientation.
//---------------------------------------------------------------------------------------------------------------------
void Board::print(int roundNumber) const
{
  if (!printing) return; // skip if printing is turned off

  bool p1OnBottom = (roundNumber == 1 || roundNumber == 4 || roundNumber == 5 ||
                     roundNumber == 8 || roundNumber == 9 || roundNumber == 12 ||
                     roundNumber == 13 || roundNumber == 16 || roundNumber == 17 ||
                     roundNumber == 20 || roundNumber == 21 || roundNumber == 24);

  // Dynamically assign zones based on who is attacker (bottom)
  const Zone &topField = p1OnBottom ? defField : atkField;
  const Zone &topBattle = p1OnBottom ? defBattle : atkBattle;
  const Zone &bottomField = p1OnBottom ? atkField : defField;
  const Zone &bottomBattle = p1OnBottom ? atkBattle : defBattle;

  int topPlayerId;
  int bottomPlayerId;

  if (roundNumber == 1 || roundNumber == 4 || roundNumber == 5 ||
      roundNumber == 8 || roundNumber == 9 || roundNumber == 12 ||
      roundNumber == 13 || roundNumber == 16 || roundNumber == 17 ||
      roundNumber == 20 || roundNumber == 21 || roundNumber == 24)
  {
    topPlayerId = 2;
    bottomPlayerId = 1;
  }
  else
  {
    topPlayerId = 1;
    bottomPlayerId = 2;
  };


  // --- Defender border line ---
  cout
      << "================================== DEFENDER: PLAYER "
      << topPlayerId
      << " ===================================\n";

  // --- Defender's Field Zone (7 slots side by side) ---
  topField.printZone();

  // --- Divider between Field and Battle zones ---
  cout
      << "===[---------]=[---------]=[---------]=[---------]"
      "=[---------]=[---------]=[---------]===\n";

  // --- Defender's Battle Zone ---
  topBattle.printZone();

  // --- Lane index markers between defender & attacker battle rows ---
  cout
      << "~~~[~~~ 1 ~~~]~[~~~ 2 ~~~]~[~~~ 3 ~~~]"
      "~[~~~ 4 ~~~]~[~~~ 5 ~~~]~[~~~ 6 ~~~]~[~~~ 7 ~~~]~~~\n";

  // --- Attacker's Battle Zone ---
  bottomBattle.printZone();

  // --- Divider between Battle and Field zones ---
  cout
      << "===[---------]=[---------]=[---------]=[---------]"
      "=[---------]=[---------]=[---------]===\n";

  // --- Attacker's Field Zone ---
  bottomField.printZone();

  // --- Attacker border line ---
  cout
      << "================================== ATTACKER: PLAYER "
      << bottomPlayerId
      << " ===================================\n";
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Provides access to the defender's field zone.
///
/// @return Reference to the defender's field zone.
//---------------------------------------------------------------------------------------------------------------------
Zone &Board::defenderField() { return defField; }
Zone &Board::defenderBattle() { return defBattle; }
Zone &Board::attackerField() { return atkField; }
Zone &Board::attackerBattle() { return atkBattle; }


//---------------------------------------------------------------------------------------------------------------------
///
/// Checks whether the given field slot is occupied by a card for the specified player.
///
/// @param playerId ID of the player (1 for attacker, 2 for defender)
/// @param slot The name of the slot to check (e.g. "A1", "B3")
///
/// @return true if the slot is occupied, false otherwise.
//---------------------------------------------------------------------------------------------------------------------
bool Board::isFieldSlotOccupied(int playerId, const std::string &slot) const
{
  const Zone &zone = (playerId == 1) ? atkField : defField;
  return zone.isOccupied(slot);
}
