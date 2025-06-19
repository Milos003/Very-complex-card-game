// --------------------------- CardFactory.cpp ---------------------------
//
// This file implements the CardFactory class, responsible for loading
// and instantiating card objects from configuration files.
// It supports creature and spell cards with traits or spell types.
//
// Group: 051
//
// Author: Miloš Đukarić, Florian Kerman, Stefan Jović
// -----------------------------------------------------------------------
#include "CardFactory.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <iostream>

using namespace std; // bring in std symbols for brevity

// -------------------------------------------------------------
// Parses a trait string and returns the corresponding Trait enum.
// Unknown traits are logged and defaulted to Brutal.
//
// @param traitStr The string representation of a trait.
// @return Corresponding Trait enum value.
// -------------------------------------------------------------
Trait CardFactory::parseTrait(const string &traitStr)
{
  if (traitStr == "Brutal") return Trait::Brutal;
  if (traitStr == "Challenger") return Trait::Challenger;
  if (traitStr == "First Strike") return Trait::FirstStrike;
  if (traitStr == "Haste") return Trait::Haste;
  if (traitStr == "Lifesteal") return Trait::Lifesteal;
  if (traitStr == "Poisoned") return Trait::Poisoned;
  if (traitStr == "Regenerate") return Trait::Regenerate;
  if (traitStr == "Temporary") return Trait::Temporary;
  if (traitStr == "Undying") return Trait::Undying;
  if (traitStr == "Venomous") return Trait::Venomous;

  cerr << "[WARNING] Unknown trait: " << traitStr << endl;
  return Trait::Brutal; // fallback trait
}

// -------------------------------------------------------------
// Loads creature card definitions from creatureCards.txt.
// Each card is parsed and added to the creatureCards map.
// Lines starting with '#' or empty lines are skipped.
// -------------------------------------------------------------
void CardFactory::loadCreatureCards()
{
  ifstream file("data/creatureCards.txt");
  if (!file.is_open())
  {
    cerr << "Failed to open creatureCards.txt" << endl;
    return;
  }

  string line;
  while (getline(file, line))
  {
    // Skip comment or empty lines
    if (line.empty() || line[0] == '#') continue;

    stringstream ss(line);
    string id, name, manaStr, atkStr, hpStr, traitsStr;
    getline(ss, id, ';');
    if (id == "ID") continue;
    getline(ss, name, ';');
    getline(ss, manaStr, ';');
    getline(ss, atkStr, ';');
    getline(ss, hpStr, ';');
    getline(ss, traitsStr, ';');

    // parsing traits in a vector
    vector<Trait> baseTraits;
    stringstream tss(traitsStr);
    string traitToken;
    while (getline(tss, traitToken, ','))
    {
      if (!traitToken.empty())
      {
        /*remove leading spaces*/
        traitToken.erase(0, traitToken.find_first_not_of(" \t\r\n"));
        traitToken.erase(traitToken.find_last_not_of(" \t\r\n") + 1);

        baseTraits.push_back(parseTrait(traitToken));
      }
    }


    // Normalize ID to uppercase
    transform(id.begin(), id.end(), id.begin(), ::toupper);

    int manaCost = 0, baseAttack = 0, baseHealth = 0;
    try
    {
      manaCost = stoi(manaStr);
      baseAttack = stoi(atkStr);
      baseHealth = stoi(hpStr);
    }
    catch (const invalid_argument &e)
    {
      cerr << "Invalid number format in line: " << line << endl;
      continue; // skip invalid entry
    } catch (const out_of_range &e)
    {
      cerr << "Number out of range in line: " << line << endl;
      continue; // skip out-of-range entry
    }

    // Example default traits; adjust as needed
    auto creature = make_shared<CreatureCard>(
      id, name, manaCost, baseAttack, baseHealth, baseTraits
    );


    creatureCards[id] = creature;
  }
}

// -------------------------------------------------------------
// Loads spell card definitions from spellCards.txt.
// Each spell is added to the spellCards map with its type.
// -------------------------------------------------------------
void CardFactory::loadSpellCards()
{
  ifstream file("data/spellCards.txt");
  if (!file.is_open())
  {
    cerr << "Failed to open spellCards.txt" << endl;
    return;
  }

  string line;
  while (getline(file, line))
  {
    if (line.empty() || line[0] == '#') continue;

    stringstream ss(line);
    string id, name, manaStr, spellTypeStr;
    getline(ss, id, ';');
    getline(ss, name, ';');
    getline(ss, manaStr, ';');
    getline(ss, spellTypeStr, ';');

    transform(id.begin(), id.end(), id.begin(), ::toupper);

    // Handle variable mana costs (e.g., X spells)
    int manaCost = (manaStr == "X" || manaStr == "x") ? -1 : stoi(manaStr);

    // Determine SpellType enum
    SpellType spellType;
    if (spellTypeStr == "General") spellType = SpellType::General;
    else if (spellTypeStr == "Target") spellType = SpellType::Target;
    else if (spellTypeStr == "Graveyard") spellType = SpellType::Graveyard;

    auto spell = make_shared<SpellCard>(id, name, manaCost, spellType);
    spellCards[id] = spell;
  }
}

// -------------------------------------------------------------
// Creates a deep copy of a card by its ID.
// Searches both creature and spell card maps.
//
// @param id Card identifier (case-insensitive).
// @return Shared pointer to a new card instance or nullptr.
// -------------------------------------------------------------
shared_ptr<Card> CardFactory::createCardByID(const string &id)
{
  string upperId = id;
  transform(upperId.begin(), upperId.end(), upperId.begin(), ::toupper);

  // Lookup in creature cards
  if (creatureCards.count(upperId))
  {
    auto basePtr = dynamic_pointer_cast<CreatureCard>(creatureCards[upperId]);
    return make_shared<CreatureCard>(*basePtr);
  }
  // Lookup in spell cards
  if (spellCards.count(upperId))
  {
    auto basePtr = dynamic_pointer_cast<SpellCard>(spellCards[upperId]);
    return make_shared<SpellCard>(*basePtr);
  }

  // Not found: return null pointer
  return nullptr;
}

// -------------------------------------------------------------
// Checks if the given card ID is valid (creature or spell).
//
// @param id Card ID to check.
// @return true if ID is found, false otherwise.
// -------------------------------------------------------------
bool CardFactory::isValidCardID(const std::string &id) const
{
  // Uppercase the input for a case-insensitive lookup
  string upperId = id;
  transform(upperId.begin(), upperId.end(), upperId.begin(), ::toupper);

  // Now do the real test
  return creatureCards.count(upperId) > 0
         || spellCards.count(upperId) > 0;
}
