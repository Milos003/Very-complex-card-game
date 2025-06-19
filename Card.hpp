// --------------------------- Card.hpp ---------------------------
//
// This file defines the abstract base class Card and the CardType enum.
// It provides common properties and interface for all card types,
// such as name, ID, mana cost, and virtual methods for printing and resetting.
//
// Group: 051
//
// Author: Miloš Đukarić, Florian Kerman, Stefan Jović
// -----------------------------------------------------------------

#ifndef CARD_HPP
#define CARD_HPP

#include <string>

using namespace std; // bring in std symbols for clarity

//-----------------------------------------------------------------------------
// CardType: defines the category of a card (creature vs. spell)
//-----------------------------------------------------------------------------
enum class CardType
{
  Creature, // creature type: has attack/defense stats
  Spell // spell type: one-time effect
};

//-----------------------------------------------------------------------------
// Card: abstract base class for all cards in the game
//-----------------------------------------------------------------------------
class Card
{
protected:
  string id; // unique identifier for the card
  string name; // display name of the card
  int manaCost; // mana required to play the card
  CardType type; // type of the card (creature/spell)

public:
  // -------------------------------------------------------------
  //
  // Constructor for a Card.
  //
  // @param id        Unique ID for this card
  // @param name      Human-readable card name
  // @param manaCost  Mana required to play
  // @param type      CardType enum value
  //
  // -------------------------------------------------------------
  Card(const string &id, const string &name, int manaCost, CardType type)
    : id(id)
      , name(name)
      , manaCost(manaCost)
      , type(type)
  {
  }

  // -------------------------------------------------------------
  //
  // Virtual destructor to allow polymorphic cleanup.
  //
  // -------------------------------------------------------------
  virtual ~Card() = default;

  // -------------------------------------------------------------
  //
  // Prints all relevant details of the card.
  // Must be overridden by derived classes.
  //
  // -------------------------------------------------------------
  virtual void printCardDetails() const = 0;

  // -------------------------------------------------------------
  //
  // Returns the unique ID of the card.
  //
  // @return string ID
  //
  // -------------------------------------------------------------
  string getID() const { return id; }

  // -------------------------------------------------------------
  //
  // Returns the name of the card.
  //
  // @return string name
  //
  // -------------------------------------------------------------
  string getName() const { return name; }

  // -------------------------------------------------------------
  //
  // Returns the mana cost required to play the card.
  //
  // @return int mana cost
  //
  // -------------------------------------------------------------
  int getManaCost() const { return manaCost; }

  // -------------------------------------------------------------
  //
  // Returns the card type (Creature or Spell).
  //
  // @return CardType enum
  //
  // -------------------------------------------------------------
  CardType getType() const { return type; }

  // -------------------------------------------------------------
  //
  // Resets mutable stats to base values.
  // Must be overridden by derived classes.
  //
  // -------------------------------------------------------------
  virtual void resetStats() = 0;
};

#endif // CARD_HPP
