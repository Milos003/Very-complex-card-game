// --------------------------- SpellCard.hpp ---------------------------
//
// Declaration of SpellCard: a subclass of Card representing playable spells.
// Spells are categorized as General, Target, or Graveyard based on behavior.
//
// Group: 051
//
// Author: Miloš Đukarić, Florian Kerman, Stefan Jović
// ------------------------------------------------------------------------
#ifndef SPELLCARD_HPP
#define SPELLCARD_HPP

#include "Card.hpp"
#include <iostream>

using namespace std; // bring in std symbols for clarity

//---------------------------------------------------------------------------------------------------------------------
///
/// Enum class representing different types of spell behaviors.
///
/// - General   → one-time effect, no specific target
/// - Target    → directed at a specific card or player
/// - Graveyard → interacts with graveyard cards
///
//---------------------------------------------------------------------------------------------------------------------
enum class SpellType
{
  General, ///< one-time effect without specific target
  Target, ///< effect directed at a specific card or player
  Graveyard ///< effect interacts with cards in the graveyard
};

//---------------------------------------------------------------------------------------------------------------------
///
/// Concrete card type representing spell cards. Inherits from Card.
/// Spells can be general, targeted, or graveyard-based, and have no persistent stats.
///
/// Used for spell-related commands and card effect resolution.
///
//---------------------------------------------------------------------------------------------------------------------
class SpellCard : public Card
{
protected:
  SpellType spellType; ///< specific type of this spell

public:
  //---------------------------------------------------------------------------------------------------------------------
  ///
  /// Constructs a new SpellCard with given ID, name, mana cost, and spell type.
  ///
  /// @param id         Unique card ID (uppercase, 5 characters)
  /// @param name       Display name of the spell
  /// @param manaCost   Mana required to cast the spell (-1 if variable/X-cost)
  /// @param spellType  Category of the spell (General, Target, or Graveyard)
  ///
  //---------------------------------------------------------------------------------------------------------------------
  SpellCard(const string &id,
            const string &name,
            int manaCost,
            SpellType spellType)
    : Card(id, name, manaCost, CardType::Spell)
      , spellType(spellType)
  {
  }

  //---------------------------------------------------------------------------------------------------------------------
  ///
  /// Returns the type of this spell.
  ///
  /// @return SpellType enum (General, Target, or Graveyard)
  ///
  //---------------------------------------------------------------------------------------------------------------------
  SpellType getSpellType() const { return spellType; }

  //---------------------------------------------------------------------------------------------------------------------
  ///
  /// Resets stats of the card. For spells, this is a no-op.
  ///
  //---------------------------------------------------------------------------------------------------------------------
  void resetStats() override
  {
    // Spells have no persistent stats to reset
  }

  //---------------------------------------------------------------------------------------------------------------------
  ///
  /// Prints an ASCII-art representation of the spell card.
  /// Includes formatted mana cost and leaves trait zone empty.
  ///
  //---------------------------------------------------------------------------------------------------------------------
  void printCardDetails() const override
  {
    // Format mana cost (XX for variable)
    string manaStr = (manaCost == -1) ? "XX" : to_string(manaCost);
    while (manaStr.length() < 2)
    {
      manaStr = "0" + manaStr;
    }

    // ID assumed uppercase and length 5
    string cardID = id;

    // Print ASCII layout
    cout << " _____M" << manaStr << endl;
    cout << "| " << cardID << " |" << endl;
    cout << "|       |" << endl;
    cout << " _______" << endl;
  }
};

#endif // SPELLCARD_HPP
