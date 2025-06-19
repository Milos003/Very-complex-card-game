// --------------------------- Zone.cpp ---------------------------
//
// Implementation of the Zone class which manages a 7-slot zone on the board,
// used for field and battle representation of cards.
//
// Group: 051
//
// Author: Miloš Đukarić, Florian Kerman, Stefan Jović
// ------------------------------------------------------------------------
#include "Zone.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std; // bring std names into this file

//---------------------------------------------------------------------------------------------------------------------
///
/// Constructs a Zone with a fixed number of slots (7) and a given marker character.
///
/// @param marker Character representing the zone (e.g., 'F' for Field, 'B' for Battle)
///
//---------------------------------------------------------------------------------------------------------------------
Zone::Zone(char marker)
  : zoneChar(marker)
    , slots(7, nullptr)
{
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Places or overwrites a card in the specified slot index (0-based).
///
/// @param index Slot index in the range [0,6]
/// @param card Shared pointer to the card to place
///
//---------------------------------------------------------------------------------------------------------------------
void Zone::addCard(int index, shared_ptr<Card> card)
{
  if (index >= 0 && index < static_cast<int>(slots.size()))
  {
    slots[index] = card;
  }
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Removes a card from the specified slot by setting it to nullptr.
///
/// @param index Slot index in the range [0,6]
///
//---------------------------------------------------------------------------------------------------------------------
void Zone::removeCard(int index)
{
  if (index >= 0 && index < static_cast<int>(slots.size()))
  {
    slots[index] = nullptr;
  }
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Clears the entire zone by removing all cards from all slots.
///
//---------------------------------------------------------------------------------------------------------------------
void Zone::clear()
{
  fill(slots.begin(), slots.end(), nullptr);
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Prints a 4-line ASCII representation of the zone, formatted as 7 slots side by side.
/// Each slot is rendered using the card's printCardDetails output.
///
//---------------------------------------------------------------------------------------------------------------------
void Zone::printZone() const
{
  const string gap = "   "; // 3-space gap
  const int cellWidth = 9;
  const string blank(cellWidth, ' ');

  // Prepare 4-line art for each of the 7 slots
  vector<array<string, 4> > art(7);
  for (int i = 0; i < 7; ++i)
  {
    if (slots[i])
    {
      // Capture the card's ASCII art via printCardDetails()
      ostringstream oss;
      auto *oldBuf = cout.rdbuf(oss.rdbuf());
      slots[i]->printCardDetails();
      cout.rdbuf(oldBuf);

      // Split into non-empty lines
      vector<string> lines;
      istringstream ls(oss.str());
      for (string ln; getline(ls, ln);)
      {
        if (!ln.empty()) lines.push_back(ln);
      }

      // Left-align each line into a cellWidth-wide string
      for (int r = 0; r < 4; ++r)
      {
        if (r < static_cast<int>(lines.size()))
        {
          const string &ln = lines[r];
          art[i][r] = ln + string(cellWidth - ln.size(), ' ');
        }
        else
        {
          art[i][r] = blank;
        }
      }
    }
    else
    {
      // Empty slot: fill entire cell with spaces
      for (int r = 0; r < 4; ++r)
      {
        art[i][r] = blank;
      }
    }
  }

  // Now print each of the 4 rows
  for (int row = 0; row < 4; ++row)
  {
    // Left marker + gap
    cout << zoneChar << gap;
    // First slot
    cout << art[0][row];
    // Remaining slots (prefix each with gap)
    for (int i = 1; i < 7; ++i)
    {
      cout << gap << art[i][row];
    }
    // Trailing gap + marker, then newline
    cout << gap << zoneChar << "\n";
  }
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Checks whether a slot (given by its label, e.g., "F1", "B7") is occupied.
///
/// @param slot String representing the zone and index (e.g. "F3")
///
/// @return true if the slot exists and contains a card, false otherwise
///
//---------------------------------------------------------------------------------------------------------------------
bool Zone::isOccupied(const std::string &slot) const
{
  if (slot.length() != 2 || slot[0] != zoneChar)
  {
    return false; // wrong zone or invalid format
  }

  int index = slot[1] - '1';
  if (index < 0 || index >= 7)
  {
    return false; // out of bounds
  }

  return slots[index] != nullptr;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Returns a raw pointer to the card at the given slot index.
///
/// @param index Slot index (0-based)
///
/// @return Pointer to card if present, otherwise nullptr
///
//---------------------------------------------------------------------------------------------------------------------
Card *Zone::getCard(int index) const
{
  if (index >= 0 && index < static_cast<int>(slots.size()))
  {
    return slots[index].get();
  }
  return nullptr;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Extracts a card from a slot, returning ownership and clearing the slot.
///
/// @param index Slot index (0-based)
///
/// @return Shared pointer to the card, or nullptr if empty or invalid
///
//---------------------------------------------------------------------------------------------------------------------
shared_ptr<Card> Zone::extractCard(int index)
{
  if (index >= 0 && index < static_cast<int>(slots.size()))
  {
    auto card = slots[index];
    slots[index] = nullptr;
    return card;
  }
  return nullptr;
}
