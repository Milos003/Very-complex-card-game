// --------------------------- CardFactory.hpp ---------------------------
//
// This file declares the CardFactory class, which is responsible for
// loading and instantiating card objects from configuration files.
// It manages both creature and spell card prototypes.
//
// Group: 051
//
// Author: Miloš Đukarić, Florian Kerman, Stefan Jović
// -----------------------------------------------------------------------
#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>
#include "Card.hpp"
#include "CreatureCard.hpp"
#include "SpellCard.hpp"

using namespace std; // bring in std symbols for clarity

// -------------------------------------------------------------
// CardFactory: responsible for loading card prototypes and
// creating new instances of cards based on ID.
// -------------------------------------------------------------
class CardFactory
{
private:
  // -------------------------------------------------------------
  // Maps that store card prototypes by uppercase ID
  // -------------------------------------------------------------
  map<string, shared_ptr<Card> > creatureCards; // Loaded creature card templates
  map<string, shared_ptr<Card> > spellCards; // Loaded spell card templates

  // -------------------------------------------------------------
  //
  // Parses a trait string and returns the corresponding enum.
  //
  // @param traitStr String representation of a trait
  // @return Trait enum
  //
  // -------------------------------------------------------------
  Trait parseTrait(const string &traitStr);

public:
  // -------------------------------------------------------------
  //
  // Loads all creature card definitions from config file into map.
  // Expected format: ID;Name;ManaCost;BaseAttack;BaseHealth;Traits
  //
  // -------------------------------------------------------------
  void loadCreatureCards();

  // -------------------------------------------------------------
  //
  // Loads all spell card definitions from config file into map.
  // Expected format: ID;Name;ManaCost;SpellType
  //
  // -------------------------------------------------------------
  void loadSpellCards();

  // -------------------------------------------------------------
  //
  // Instantiates a new Card object by given ID. Performs a case-
  // insensitive lookup and returns a deep copy of the prototype.
  //
  // @param id Card identifier (e.g., "ZMBFY")
  // @return shared_ptr<Card> if found, otherwise nullptr
  //
  // -------------------------------------------------------------
  shared_ptr<Card> createCardByID(const string &id);

  // -------------------------------------------------------------
  //
  // Checks if the given card ID is valid (present in either map).
  //
  // @param id Card identifier
  // @return true if valid, false otherwise
  //
  // -------------------------------------------------------------
  bool isValidCardID(const std::string &id) const;
};
