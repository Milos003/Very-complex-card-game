// --------------------------- Zone.hpp ---------------------------
//
// Declaration of Zone: a class representing a 7-slot card zone (field or battle zone),
// capable of rendering cards in ASCII format and managing placement/removal logic.
//
// Group: 051
//
// Author: Miloš Đukarić, Florian Kerman, Stefan Jović
// ------------------------------------------------------------------------
#pragma once

#include <vector>
#include <array>
#include <memory>
#include <string>
#include "Card.hpp"

using namespace std; // bring std names into this header for brevity

//---------------------------------------------------------------------------------------------------------------------
///
/// Represents a 7-slot zone (e.g., Field or Battle zone) for holding and displaying cards.
/// Each zone is identified by a single marker character (e.g., 'F' or 'B') used in printed borders.
/// Supports card insertion, removal, rendering, and querying.
///
//---------------------------------------------------------------------------------------------------------------------
class Zone
{
public:
  //---------------------------------------------------------------------------------------------------------------------
  ///
  /// Constructs a Zone with a character used for ASCII borders.
  ///
  /// @param marker Single character label for the zone's left/right border (e.g., 'F' or 'B')
  ///
  //---------------------------------------------------------------------------------------------------------------------
  explicit Zone(char marker);

  //---------------------------------------------------------------------------------------------------------------------
  ///
  /// Places or overwrites a card in a given slot (0–6).
  ///
  /// @param index 0-based slot index
  /// @param card Shared pointer to a Card to insert; nullptr empties the slot
  ///
  //---------------------------------------------------------------------------------------------------------------------
  void addCard(int index, shared_ptr<Card> card);

  //---------------------------------------------------------------------------------------------------------------------
  ///
  /// Removes any card from the specified slot.
  ///
  /// @param index 0-based slot index
  ///
  //---------------------------------------------------------------------------------------------------------------------
  void removeCard(int index);

  //---------------------------------------------------------------------------------------------------------------------
  ///
  /// Clears the entire zone by removing all cards from slots.
  ///
  //---------------------------------------------------------------------------------------------------------------------
  void clear();

  //---------------------------------------------------------------------------------------------------------------------
  ///
  /// Prints the zone as a 4-row ASCII art representation.
  ///
  /// - Each slot is 9 characters wide
  /// - 3 spaces separate slots
  /// - Zone edges are marked with zoneChar
  ///
  //---------------------------------------------------------------------------------------------------------------------
  void printZone() const;

  //---------------------------------------------------------------------------------------------------------------------
  ///
  /// Retrieves a raw pointer to the card in the given slot.
  ///
  /// @param index 0-based index of the slot
  ///
  /// @return Pointer to the Card if present, nullptr otherwise
  ///
  //---------------------------------------------------------------------------------------------------------------------
  Card *getCard(int index) const;

  //---------------------------------------------------------------------------------------------------------------------
  ///
  /// Extracts a card from a slot, transferring ownership and clearing the slot.
  ///
  /// @param index 0-based slot index
  ///
  /// @return Shared pointer to the extracted card, or nullptr if empty
  ///
  //---------------------------------------------------------------------------------------------------------------------
  shared_ptr<Card> extractCard(int index);

  //---------------------------------------------------------------------------------------------------------------------
  ///
  /// Checks whether a given labeled slot (e.g., "F3") is currently occupied.
  ///
  /// @param slot String representation of the slot label (must match zoneChar + [1–7])
  ///
  /// @return true if slot is valid and contains a card, false otherwise
  ///
  //---------------------------------------------------------------------------------------------------------------------
  bool isOccupied(const std::string &slot) const;

private:
  char zoneChar; // Border character on each row start/end
  vector<shared_ptr<Card> > slots; // Exactly 7 card pointers (may be nullptr)
};
