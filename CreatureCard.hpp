// --------------------------- CreatureCard.hpp ---------------------------
//
// This file defines the CreatureCard class, which extends the base Card
// and models creatures with health, attack, and traits such as Haste,
// Lifesteal, Undying, etc. Also includes the Trait enum and related helpers.
//
// Group: 051
//
// Author: Miloš Đukarić, Florian Kerman, Stefan Jović
// ------------------------------------------------------------------------
#ifndef CREATURECARD_HPP
#define CREATURECARD_HPP

#include "Card.hpp"
#include <vector>
#include <algorithm>
#include <iostream>
#include <iomanip>

using namespace std; // bring in std symbols for clarity

//-----------------------------------------------------------------------------
// Trait: flags indicating special abilities or statuses for creature cards
//-----------------------------------------------------------------------------
enum class Trait
{
  Brutal, ///< deals extra damage
  Challenger, ///< forces blocks
  FirstStrike, ///< attacks before defenders
  Haste, ///< can attack immediately
  Lifesteal, ///< heals controller on damage
  Poisoned, ///< applies poison effect
  Regenerate, ///< recovers health after destruction
  Temporary, ///< limited-duration buffs
  Undying, ///< returns with 1 HP when destroyed
  Venomous ///< applies poison on attack
};

// -------------------------------------------------------------
// Converts a Trait enum value into a full string name.
// -------------------------------------------------------------
inline std::string traitToString(Trait t)
{
  switch (t)
  {
    case Trait::Brutal: return "Brutal";
    case Trait::Challenger: return "Challenger";
    case Trait::FirstStrike: return "First Strike";
    case Trait::Haste: return "Haste";
    case Trait::Lifesteal: return "Lifesteal";
    case Trait::Poisoned: return "Poisoned";
    case Trait::Regenerate: return "Regenerate";
    case Trait::Temporary: return "Temporary";
    case Trait::Undying: return "Undying";
    case Trait::Venomous: return "Venomous";
    default: return "";
  }
}

// -------------------------------------------------------------
// CreatureCard: represents a creature card with traits, attack, and health
// -------------------------------------------------------------
class CreatureCard : public Card
{
protected:
  int baseATK;
  int baseHP;
  int curATK;
  int curHP;
  vector<Trait> baseTraits;
  vector<Trait> traits;
  int summonedRound;
  int lastFieldIndex = -1;
  int lastFieldOwner = -1;
  bool resurrected = false;

public:
  // -------------------------------------------------------------
  //
  // Constructor for a CreatureCard.
  //
  // @param id          Card ID
  // @param name        Card name
  // @param manaCost    Mana cost
  // @param baseATK     Base attack value
  // @param baseHP      Base health value
  // @param baseTraits  Initial traits for the creature
  //
  // -------------------------------------------------------------
  CreatureCard(
    const string &id,
    const string &name,
    int manaCost,
    int baseATK,
    int baseHP,
    const vector<Trait> &baseTraits
  )
    : Card(id, name, manaCost, CardType::Creature)
      , baseATK(baseATK)
      , baseHP(baseHP)
      , curATK(baseATK)
      , curHP(baseHP)
      , baseTraits(baseTraits)
      , traits(baseTraits)
      , summonedRound(-1)
  {
  }

  // --------------------------------------------------------------------------
  // Removes all instances of the given trait from the creature.
  //
  // @param trait The trait to remove from the current traits list
  // --------------------------------------------------------------------------
  void removeTrait(Trait trait)
  {
    traits.erase(std::remove(traits.begin(), traits.end(), trait), traits.end());
  }

  // Returns sorted single-letter trait code string (max 5 letters; 5th is '+' if overflow)
  string getTraitsString() const
  {
    vector<char> traitLetters;
    for (Trait t: traits)
    {
      switch (t)
      {
        case Trait::Brutal: traitLetters.push_back('B');
          break;
        case Trait::Challenger: traitLetters.push_back('C');
          break;
        case Trait::FirstStrike: traitLetters.push_back('F');
          break;
        case Trait::Haste: traitLetters.push_back('H');
          break;
        case Trait::Lifesteal: traitLetters.push_back('L');
          break;
        case Trait::Poisoned: traitLetters.push_back('P');
          break;
        case Trait::Regenerate: traitLetters.push_back('R');
          break;
        case Trait::Temporary: traitLetters.push_back('T');
          break;
        case Trait::Undying: traitLetters.push_back('U');
          break;
        case Trait::Venomous: traitLetters.push_back('V');
          break;
      }
    }

    sort(traitLetters.begin(), traitLetters.end());

    string result;
    for (size_t i = 0; i < min<size_t>(5, traitLetters.size()); ++i)
    {
      result += traitLetters[i];
    }
    if (traitLetters.size() > 5)
    {
      result[4] = '+';
    }

    return result;
  }

  const std::vector<Trait> &getBaseTraits() const
  {
    return baseTraits;
  }


  const std::vector<Trait> &getTraits() const
  {
    return traits;
  }

  // --------------------------------------------------------------------------
  // Sets the last field index this creature was placed in.
  //
  // @param index Index in the field zone (0-6)
  // --------------------------------------------------------------------------
  void setLastFieldIndex(int index)
  {
    lastFieldIndex = index;
  }

  // --------------------------------------------------------------------------
  // Checks whether this creature was resurrected by Undying.
  //
  // @return true if resurrected, false otherwise
  // --------------------------------------------------------------------------
  bool isResurrected() const { return resurrected; }

  // --------------------------------------------------------------------------
  // Marks the creature as resurrected (typically by Undying).
  // --------------------------------------------------------------------------
  void markResurrected() { resurrected = true; }

  // --------------------------------------------------------------------------
  // Gets the index of the last field slot the creature occupied.
  //
  // @return Field slot index (0-6), or -1 if unset
  // --------------------------------------------------------------------------
  int getLastFieldIndex() const
  {
    return lastFieldIndex;
  }

  // --------------------------------------------------------------------------
  // Sets the owner ID of the last field the creature was in.
  //
  // @param id Player ID (1 or 2)
  // --------------------------------------------------------------------------
  void setLastFieldOwner(int id)
  {
    lastFieldOwner = id;
  }

  // --------------------------------------------------------------------------
  // Gets the ID of the player who last controlled this card in the field.
  //
  // @return Player ID (1 or 2), or -1 if unset
  // --------------------------------------------------------------------------
  int getLastFieldOwner() const
  {
    return lastFieldOwner;
  }

  // --------------------------------------------------------------------------
  // Gets the creature's current attack value.
  //
  // @return Current attack
  // --------------------------------------------------------------------------
  int getCurrentATK() const
  {
    return curATK;
  }

  // --------------------------------------------------------------------------
  // Gets the creature's current health value.
  //
  // @return Current HP
  // --------------------------------------------------------------------------
  int getCurrentHP() const
  {
    return curHP;
  }

  // --------------------------------------------------------------------------
  // Gets the creature's base (original) attack value.
  //
  // @return Base attack
  // --------------------------------------------------------------------------
  int getBaseATK() const
  {
    return baseATK;
  }

  // --------------------------------------------------------------------------
  // Gets the creature's base (original) health value.
  //
  // @return Base HP
  // --------------------------------------------------------------------------
  int getBaseHP() const
  {
    return baseHP;
  }

  // --------------------------------------------------------------------------
  // Alias for getCurrentATK().
  //
  // @return Current attack
  // --------------------------------------------------------------------------
  int getAttack() const
  {
    return curATK;
  }

  // --------------------------------------------------------------------------
  // Alias for getCurrentHP().
  //
  // @return Current HP
  // --------------------------------------------------------------------------
  int getHealth() const
  {
    return curHP;
  }

  // --------------------------------------------------------------------------
  // Checks whether the creature has a specific trait.
  //
  // @param t Trait to check for
  // @return true if present, false otherwise
  // --------------------------------------------------------------------------
  bool hasTrait(Trait t) const
  {
    return std::find(traits.begin(), traits.end(), t) != traits.end();
  }

  // --------------------------------------------------------------------------
  // Sets the round number in which the creature was summoned.
  //
  // @param round Current round number
  // --------------------------------------------------------------------------
  void setSummonedRound(int round)
  {
    summonedRound = round;
  }

  // --------------------------------------------------------------------------
  // Deals damage to the creature by reducing its health.
  //
  // @param amount Damage amount
  // --------------------------------------------------------------------------
  void takeDamage(int amount)
  {
    curHP -= amount;
    if (curHP < 0)
    {
      curHP = 0;
    }
  }

  // --------------------------------------------------------------------------
  // Gets the round number in which this creature was summoned.
  //
  // @return Round number
  // --------------------------------------------------------------------------
  int getSummonedRound() const
  {
    return summonedRound;
  }

  // --------------------------------------------------------------------------
  // Resets the creature's current attack, health, and traits to base values.
  // Called when the creature enters play or is revived.
  // --------------------------------------------------------------------------
  void resetStats() override
  {
    curATK = baseATK;
    curHP = baseHP;
    traits = baseTraits;
  }

  // --------------------------------------------------------------------------
  // Adds a trait to the creature if it doesn't already have it.
  //
  // @param t Trait to add
  // --------------------------------------------------------------------------
  void addTrait(Trait t)
  {
    if (!hasTrait(t))
    {
      traits.push_back(t);
    }
  }

  // --------------------------------------------------------------------------
  // Increases the creature's current attack value.
  //
  // @param amount Amount to increase
  // --------------------------------------------------------------------------
  void increaseAttack(int amount)
  {
    curATK += amount;
  }

  // --------------------------------------------------------------------------
  // Increases the creature's current health value.
  //
  // @param amount Amount to increase
  // --------------------------------------------------------------------------
  void increaseHealth(int amount)
  {
    curHP += amount;
  }

  // --------------------------------------------------------------------------
  // Decreases the creature's current health value, not below zero.
  //
  // @param amount Amount to subtract
  // --------------------------------------------------------------------------
  void decreaseHealth(int amount)
  {
    curHP -= amount;
    if (curHP < 0)
    {
      curHP = 0;
    }
  }

  // --------------------------------------------------------------------------
  // Sets the creature's current health to a specific value.
  //
  // @param hp New health value
  // --------------------------------------------------------------------------
  void setHealth(int hp)
  {
    curHP = hp;
  }

  // --------------------------------------------------------------------------
  // Removes the first trait from the creature's traits list
  // (ordered alphabetically by trait name).
  // --------------------------------------------------------------------------
  void removeFirstTraitAlphabetically()
  {
    if (!traits.empty())
    {
      sort(traits.begin(), traits.end(), [](Trait a, Trait b)
      {
        return traitToString(a) < traitToString(b);
      });
      traits.erase(traits.begin());
    }
  }

  // Prints all key details of the card (ID, traits, mana, attack, health).
  // Used when displaying the card in ASCII board format.
  // --------------------------------------------------------------------------
  void printCardDetails() const override
  {
    string manaStr = (manaCost == -1) ? "XX" : to_string(manaCost);
    while (manaStr.length() < 2)
    {
      manaStr = "0" + manaStr;
    }

    string atkStr = (curATK == -1) ? "XX" : to_string(curATK);
    while (atkStr.length() < 2)
    {
      atkStr = "0" + atkStr;
    }

    string hpStr = (curHP == -1) ? "XX" : to_string(curHP);
    while (hpStr.length() < 2)
    {
      hpStr = "0" + hpStr;
    }

    string traitLetters = getTraitsString();
    while (traitLetters.length() < 5)
    {
      traitLetters += " ";
    }


    string cardID = id;

    cout << " _____M" << manaStr << endl;
    cout << "| " << cardID << " |" << endl;
    cout << "| " << traitLetters << " |" << endl;
    cout << "A" << atkStr << "___H" << hpStr << endl;
  }
};

#endif // CREATURECARD_HPP
