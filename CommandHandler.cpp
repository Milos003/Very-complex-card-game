// --------------------------- CommandHandler.cpp ---------------------------
//
// This file implements the CommandHandler class, which processes all user
// commands in the card game. It handles parsing, validation, and execution
// of actions such as playing cards, ending turns, and printing game state.
//
// Group: 051
//
// Author: Miloš Đukarić, Florian Kerman, Stefan Jović
// --------------------------------------------------------------------------
#include "CommandHandler.hpp"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>
#include <regex>

using namespace std;

// Main process dispatcher
bool CommandHandler::process(const std::string &rawInput, Game &game)
{
  // 1) Trim leading and trailing whitespace (spaces, tabs, CR/LF)
  string input = rawInput;
  input.erase(0, input.find_first_not_of(" \t\r\n"));
  input.erase(input.find_last_not_of(" \t\r\n") + 1);

  // 2) Normalize to lowercase for case-insensitive matching (but keep original for IDs)
  string lowered = input;
  transform(lowered.begin(), lowered.end(), lowered.begin(), ::tolower);

  if (lowered.rfind("quit", 0) == 0) return handleQuit(input, game);
  if (lowered.rfind("done", 0) == 0) return handleDone(input, game);
  if (lowered.rfind("info", 0) == 0) return handleInfo(input, game);
  if (lowered.rfind("help", 0) == 0) return handleHelp(input, game);
  if (lowered.rfind("board", 0) == 0) return handleBoard(input, game);
  if (lowered.rfind("status", 0) == 0) return handleStatus(input, game);
  if (lowered.rfind("graveyard", 0) == 0) return handleGraveyard(input, game);
  if (lowered.rfind("creature", 0) == 0) return handleCreature(input, game);
  if (lowered.rfind("battle", 0) == 0) return handleBattle(input, game);
  if (lowered.rfind("hand", 0) == 0) return handleHand(input, game);
  if (lowered.rfind("redraw", 0) == 0) return handleRedraw(input, game);
  if (lowered.rfind("spell", 0) == 0) return handleSpell(input, game);

  return printUnknownCommand(input, game);
}

// Handles "quit" command
bool CommandHandler::handleQuit(const std::string &input, Game &game)
{
  string trimmed = input.substr(4);
  if (trimmed.find_first_not_of(" \t\r\n") != string::npos)
  {
    cout << game.getMessages().getMessage("E_INVALID_PARAM_COUNT");
    return true;
  }
  return false;
}

// Handles "done" command
bool CommandHandler::handleDone(const std::string &input, Game &game)
{
  string trimmed = input.substr(4);
  if (trimmed.find_first_not_of(" \t\r\n") != string::npos)
  {
    cout << game.getMessages().getMessage("E_INVALID_PARAM_COUNT");
    return true;
  }
  game.doneCounter++;
  Player &currentPlayer = game.getCurrentPlayer();
  Zone &battleZone = (currentPlayer.getId() == 1)
                       ? game.getBoard().attackerField()
                       : game.getBoard().defenderField();
  // Regenerate effect (odd rounds)
  if (game.getCurrentRound() % 2 == 1)
  {
    for (int i = 0; i < 7; ++i)
    {
      Card *card = battleZone.getCard(i);
      CreatureCard *creature = dynamic_cast<CreatureCard *>(card);
      if (creature &&
          creature->hasTrait(Trait::Regenerate) &&
          creature->getHealth() < creature->getBaseHP())
      {
        creature->setHealth(creature->getBaseHP());
        std::cout << game.getMessages().getMessage("I_REGENERATE");
      }
    }
  }
  // Poisoned effect
  for (int i = 0; i < 7; ++i)
  {
    Card *card = battleZone.getCard(i);
    CreatureCard *creature = dynamic_cast<CreatureCard *>(card);
    if (creature && creature->hasTrait(Trait::Poisoned))
    {
      creature->decreaseHealth(1);
      std::cout << game.getMessages().getMessage("I_POISONED");
      if (creature->getHealth() <= 0)
      {
        shared_ptr<Card> dead = battleZone.extractCard(i);
        currentPlayer.addToGraveyard(std::dynamic_pointer_cast<CreatureCard>(dead));
      }
    }
  }
  if (game.doneCounter == 2)
  {
    // Only now both players have played "done"
    game.initRound(); // prints boards
    game.processBattlePhase();
    if (game.isGameOver()) return false;
    game.incrementRound();
    if (game.isGameOver()) return false;
    game.updateRolesForNewRound();
    game.printRoundHeader();
    game.initRound();
    game.doneCounter = 0;
  }
  else
  {
    game.initRound();
    game.switchPlayer();
  }
  return !game.isGameOver();
}

// Handles "info" command
bool CommandHandler::handleInfo(const std::string &input, Game &game)
{
  vector<string> parts;
  stringstream ss(input);
  string word;
  while (ss >> word) parts.push_back(word);

  if (parts.size() != 2)
  {
    cout << game.getMessages().getMessage("E_INVALID_PARAM_COUNT");
    return true;
  }

  string cardId = parts[1];
  transform(cardId.begin(), cardId.end(), cardId.begin(), ::toupper);

  auto card = game.getCardFactory().createCardByID(cardId);
  if (!card)
  {
    std::cout << game.getMessages().getMessage("E_INVALID_CARD");
    return true;
  }

  std::cout << game.getMessages().getMessage("D_BORDER_INFO");
  if (card->getType() == CardType::Creature)
  {
    auto creature = dynamic_cast<CreatureCard *>(card.get());
    std::cout << creature->getName() << " [" << creature->getID() << "] ("
        << creature->getManaCost() << " mana)" << std::endl;
    std::cout << "Type: Creature\n";
    std::cout << "Base Attack: " << creature->getBaseATK() << std::endl;
    std::cout << "Base Health: " << creature->getBaseHP() << std::endl;
    const auto &baseTraits = creature->getBaseTraits();
    std::cout << "Base Traits: ";
    if (baseTraits.empty())
    {
      std::cout << "-\n";
    }
    else
    {
      for (size_t i = 0; i < baseTraits.size(); ++i)
      {
        std::cout << (i ? ", " : "") << traitToString(baseTraits[i]);
      }
      std::cout << "\n";
    }
  }
  else if (card->getType() == CardType::Spell)
  {
    int cost = card->getManaCost();
    std::string costDisplay = (cost >= 0) ? std::to_string(cost) : "XX";
    std::cout << card->getName() << " [" << card->getID() << "] ("
        << costDisplay << " mana)" << std::endl;
    std::cout << "Type: Spell\n";
    std::string effectKey = "D_" + card->getID();
    std::cout << "Effect: " << game.getMessages().getMessage(effectKey);
  }
  std::cout << game.getMessages().getMessage("D_BORDER_D");
  return true;
}

// Handles "help" command
bool CommandHandler::handleHelp(const std::string &input, Game &game)
{
  string trimmed = input.substr(4);
  if (trimmed.find_first_not_of(" \t\r\n") != string::npos)
  {
    cout << game.getMessages().getMessage("E_INVALID_PARAM_COUNT");
    return true;
  }
  cout << R"(=== Commands ============================================================================
- help
    Prints this help text.

- quit
    Terminates the game.

- battle <FIELD_SLOT> <BATTLE_SLOT>
    Moves a creature from a Field Zone slot into a Battle Zone slot.
    <FIELD_SLOT>: Current slot of the creature (F1, ..., F7)
    <BATTLE_SLOT>: Battle slot for the creature (B1, ..., B7)

- board
    Toggles the board printing.

- creature <HAND_CARD_ID> <FIELD_SLOT>
    Places a creature from your hand into a Field Zone slot.
    <HAND_CARD_ID>: The ID of the creature card in your hand
    <FIELD_SLOT>: The Field Zone slot to place the creature in

- done
    Ends your turn for this round.

- graveyard
    Prints all the cards in your graveyard.

- hand
    Prints your hand cards.

- info <CARD_ID>
    Prints card information.
    <CARD_ID>: The ID of the card to be inspected

- redraw
    Discards all hand cards and draws the same amount minus one from your deck.

- spell <HAND_CARD_ID> [<TARGET_SLOT>|<GRAVEYARD_CARD_ID>]
    Casts a spell from your hand.
    <HAND_CARD_ID>: The ID of the spell in your hand
    <TARGET_SLOT>: The slot to target with a target spell
    <GRAVEYARD_CARD_ID>: The ID of a card in the graveyard to cast a graveyard spell on

- status
    Prints general information about both players.

=========================================================================================
)";
  return true;
}

// Handles "board" command
bool CommandHandler::handleBoard(const std::string &input, Game &game)
{
  string trimmed = input.substr(5);
  if (trimmed.find_first_not_of(" \t\r\n") != string::npos)
  {
    cout << game.getMessages().getMessage("E_INVALID_PARAM_COUNT");
    return true;
  }

  bool wasOn = game.isBoardPrinting();
  game.setBoardPrinting(!wasOn);
  if (!wasOn)
  {
    game.printBoard();
  }
  return true;
}

// Handles "status" command
bool CommandHandler::handleStatus(const std::string &input, Game &game)
{
  string trimmed = input.substr(6);
  if (trimmed.find_first_not_of(" \t\r\n") != string::npos)
  {
    cout << game.getMessages().getMessage("E_INVALID_PARAM_COUNT");
    return true;
  }
  const Player &p1 = game.getPlayer1();
  const Player &p2 = game.getPlayer2();
  bool p1IsAttacker = (&p1 == &game.getAttacker());
  bool p2IsAttacker = (&p2 == &game.getAttacker());
  cout << game.getMessages().getMessage("D_BORDER_STATUS"); {
    auto &p = game.getPlayer1();
    cout << "Player " << p.getId() << "\n"
        << "Role: " << (p1IsAttacker ? "Attacker" : "Defender") << "\n"
        << "Health: " << p.getHealth() << "\n"
        << "Mana: " << p.getMana()
        << " / " << p.getManaPoolSize() << "\n"
        << "Remaining Deck: "
        << p.getDeckRemaining() << " card(s)\n"
        << "Graveyard Size: "
        << p.getGraveyard().size() << " card(s)\n";
  }
  cout << game.getMessages().getMessage("D_BORDER_C"); {
    auto &p = game.getPlayer2();
    cout << "Player " << p.getId() << "\n"
        << "Role: " << (p2IsAttacker ? "Attacker" : "Defender") << "\n"
        << "Health: " << p.getHealth() << "\n"
        << "Mana: " << p.getMana()
        << " / " << p.getManaPoolSize() << "\n"
        << "Remaining Deck: "
        << p.getDeckRemaining() << " card(s)\n"
        << "Graveyard Size: "
        << p.getGraveyard().size() << " card(s)\n";
  }
  cout << game.getMessages().getMessage("D_BORDER_D");
  return true;
}

// Handles "graveyard" command
bool CommandHandler::handleGraveyard(const std::string &input, Game &game)
{
  string trimmed = input.substr(9);
  if (trimmed.find_first_not_of(" \t\r\n") != string::npos)
  {
    cout << game.getMessages().getMessage("E_INVALID_PARAM_COUNT");
    return true;
  }
  Player &player = game.getCurrentPlayer();
  cout << game.getMessages().getMessage("D_BORDER_GRAVEYARD");
  const auto &graveyard = player.getGraveyard();
  if (!graveyard.empty())
  {
    for (auto it = graveyard.rbegin(); it != graveyard.rend(); ++it)
    {
      CreatureCard *card = it->get();
      cout << card->getID() << " | " << card->getName() << "\n";
    }
  }
  cout << game.getMessages().getMessage("D_BORDER_D");
  return true;
}

// Handles "creature" command
bool CommandHandler::handleCreature(const std::string &input, Game &game)
{
  vector<string> parts;
  size_t pos = 0;
  string token;
  string rest = input;
  while ((pos = rest.find(' ')) != string::npos)
  {
    token = rest.substr(0, pos);
    if (!token.empty()) parts.push_back(token);
    rest.erase(0, pos + 1);
  }
  if (!rest.empty()) parts.push_back(rest);
  if (parts.size() != 3)
  {
    cout << game.getMessages().getMessage("E_INVALID_PARAM_COUNT");
    return true;
  }
  string cardId = parts[1];
  string fieldSlot = parts[2];
  transform(cardId.begin(), cardId.end(), cardId.begin(), ::toupper);
  transform(fieldSlot.begin(), fieldSlot.end(), fieldSlot.begin(), ::toupper);

  if (!game.getCardFactory().isValidCardID(cardId))
  {
    cout << game.getMessages().getMessage("E_INVALID_CARD");
    return true;
  }
  if (fieldSlot.size() != 2 || (fieldSlot[0] != 'F' && fieldSlot[0] != 'B') || fieldSlot[1] < '1' || fieldSlot[1] > '7')
  {
    cout << game.getMessages().getMessage("E_INVALID_SLOT");
    return true;
  }
  Card *card = game.getCurrentPlayer().findCardInHandById(cardId);
  if (!card)
  {
    cout << game.getMessages().getMessage("E_NOT_IN_HAND");
    return true;
  }
  if (card->getType() != CardType::Creature)
  {
    cout << game.getMessages().getMessage("E_NOT_CREATURE");
    return true;
  }
  if (fieldSlot[0] != 'F')
  {
    cout << game.getMessages().getMessage("E_NOT_IN_FIELD");
    return true;
  }
  int playerId = game.getCurrentPlayer().getId();
  if (game.getBoard().isFieldSlotOccupied(playerId, fieldSlot))
  {
    cout << game.getMessages().getMessage("E_FIELD_OCCUPIED");
    return true;
  }
  int manaCost = card->getManaCost();
  if (manaCost > game.getCurrentPlayer().getMana())
  {
    cout << game.getMessages().getMessage("E_NOT_ENOUGH_MANA");
    return true;
  }
  game.getCurrentPlayer().disableRedraw();
  Player &player = game.getCurrentPlayer();
  player.subtractMana(manaCost);
  std::shared_ptr<Card> creaturePtr = player.extractCardFromHand(card);
  CreatureCard *creature = dynamic_cast<CreatureCard *>(creaturePtr.get());
  creature->resetStats();
  int index = fieldSlot[1] - '1';
  creature->setSummonedRound(game.getCurrentRound());
  if (player.getId() == 1) game.getBoard().attackerField().addCard(index, creaturePtr);
  else game.getBoard().defenderField().addCard(index, creaturePtr);
  cout << game.getMessages().getMessage("I_" + creature->getID());
  return true;
}

// Handles "battle" command
bool CommandHandler::handleBattle(const std::string &input, Game &game)
{
  vector<string> parts;
  stringstream ss(input);
  string word;
  while (ss >> word) parts.push_back(word);
  if (parts.size() != 3)
  {
    cout << game.getMessages().getMessage("E_INVALID_PARAM_COUNT");
    return true;
  }
  string fieldSlot = parts[1];
  string battleSlot = parts[2];
  transform(fieldSlot.begin(), fieldSlot.end(), fieldSlot.begin(), ::toupper);
  transform(battleSlot.begin(), battleSlot.end(), battleSlot.begin(), ::toupper);

  auto isValidSlot = [](const string &slot)
  {
    return slot.size() == 2 &&
           (slot[0] == 'F' || slot[0] == 'B') &&
           slot[1] >= '1' && slot[1] <= '7';
  };
  if (!isValidSlot(fieldSlot) || !isValidSlot(battleSlot))
  {
    cout << game.getMessages().getMessage("E_INVALID_SLOT");
    return true;
  }
  Player &player = game.getCurrentPlayer();
  int playerId = player.getId();
  if (fieldSlot[0] != 'F')
  {
    cout << game.getMessages().getMessage("E_NOT_IN_FIELD");
    return true;
  }
  int fieldIndex = fieldSlot[1] - '1';
  Zone &fieldZone = (playerId == 1)
                      ? game.getBoard().attackerField()
                      : game.getBoard().defenderField();
  Card *fieldCard = fieldZone.getCard(fieldIndex);
  if (fieldCard == nullptr)
  {
    cout << game.getMessages().getMessage("E_FIELD_EMPTY");
    return true;
  }
  CreatureCard *creature = dynamic_cast<CreatureCard *>(fieldCard);
  int currentRound = game.getCurrentRound();
  if (creature->getSummonedRound() == currentRound && !creature->hasTrait(Trait::Haste))
  {
    cout << game.getMessages().getMessage("E_CREATURE_CANNOT_BATTLE");
    return true;
  }
  if (battleSlot[0] != 'B')
  {
    cout << game.getMessages().getMessage("E_NOT_IN_BATTLE");
    return true;
  }
  int battleIndex = battleSlot[1] - '1';
  Zone &battleZone = (playerId == 1)
                       ? game.getBoard().attackerBattle()
                       : game.getBoard().defenderBattle();
  if (battleZone.getCard(battleIndex) != nullptr)
  {
    cout << game.getMessages().getMessage("E_BATTLE_OCCUPIED");
    return true;
  }
  game.getCurrentPlayer().disableRedraw();
  shared_ptr<Card> cardPtr = fieldZone.extractCard(fieldIndex);
  CreatureCard *movedCreature = dynamic_cast<CreatureCard *>(cardPtr.get());
  if (movedCreature)
  {
    movedCreature->setLastFieldOwner(playerId);
  }
  battleZone.addCard(battleIndex, cardPtr);

  // Challenger trait logic
  if (movedCreature && movedCreature->hasTrait(Trait::Challenger))
  {
    Zone &opponentField = (playerId == 1)
                            ? game.getBoard().defenderField()
                            : game.getBoard().attackerField();

    Zone &opponentBattle = (playerId == 1)
                             ? game.getBoard().defenderBattle()
                             : game.getBoard().attackerBattle();

    Card *opponentFieldCard = opponentField.getCard(battleIndex);
    Card *opponentBattleCard = opponentBattle.getCard(battleIndex);

    if (opponentFieldCard != nullptr && opponentBattleCard == nullptr &&
        opponentFieldCard->getType() == CardType::Creature)
    {
      shared_ptr<Card> moved = opponentField.extractCard(battleIndex);
      opponentBattle.addCard(battleIndex, moved);
      cout << game.getMessages().getMessage("I_CHALLENGER");
    }
  }
  if (creature->getSummonedRound() == game.getCurrentRound() && creature->hasTrait(Trait::Haste))
  {
    cout << game.getMessages().getMessage("I_HASTE");
  }
  return true;
}

// Handles "hand" command
bool CommandHandler::handleHand(const std::string &input, Game &game)
{
  string trimmed = input.substr(4);
  if (trimmed.find_first_not_of(" \t\r\n") != string::npos)
  {
    cout << game.getMessages().getMessage("E_INVALID_PARAM_COUNT");
    return true;
  }
  Player &player = game.getCurrentPlayer();
  const auto &hand = player.getHand();
  cout << game.getMessages().getMessage("D_BORDER_HAND");
  if (!hand.empty())
  {
    player.printHand();
  }
  cout << game.getMessages().getMessage("D_BORDER_D");
  return true;
}

// Handles "redraw" command
bool CommandHandler::handleRedraw(const std::string &input, Game &game)
{
  string trimmed = input.substr(6);
  if (trimmed.find_first_not_of(" \t\r\n") != string::npos)
  {
    cout << game.getMessages().getMessage("E_INVALID_PARAM_COUNT");
    return true;
  }
  Player &player = game.getCurrentPlayer();
  if (!player.canRedraw())
  {
    if (player.getHand().size() < 2) cout << game.getMessages().getMessage("E_REDRAW_NOT_ENOUGH_CARDS");
    else cout << game.getMessages().getMessage("E_REDRAW_DISABLED");
    return true;
  }
  player.performRedraw();
  return true;
}

// Handles "spell" command
bool CommandHandler::handleSpell(const std::string &input, Game &game)
{
  vector<string> parts;
  stringstream ss(input);
  string word;
  while (ss >> word) parts.push_back(word);
  if (parts.size() < 2)
  {
    cout << game.getMessages().getMessage("E_MISSING_CARD");
    return true;
  }
  string cardId = parts[1];
  transform(cardId.begin(), cardId.end(), cardId.begin(), ::toupper);
  if (!game.getCardFactory().isValidCardID(cardId))
  {
    cout << game.getMessages().getMessage("E_INVALID_CARD");
    return true;
  }
  Card *card = game.getCurrentPlayer().findCardInHandById(cardId);
  if (!card)
  {
    cout << game.getMessages().getMessage("E_NOT_IN_HAND");
    return true;
  }
  if (card->getType() != CardType::Spell)
  {
    cout << game.getMessages().getMessage("E_NOT_SPELL");
    return true;
  }
  SpellCard *spell = dynamic_cast<SpellCard *>(card);
  SpellType type = spell->getSpellType();
  if ((type == SpellType::General && parts.size() != 2) ||
      (type != SpellType::General && parts.size() != 3))
  {
    cout << game.getMessages().getMessage("E_INVALID_PARAM_COUNT_SPELL");
    return true;
  }
  Player &player = game.getCurrentPlayer();
  int manaCost = spell->getManaCost();
  if (manaCost > player.getMana())
  {
    cout << game.getMessages().getMessage("E_NOT_ENOUGH_MANA");
    return true;
  }

  // General spells
  if (type == SpellType::General)
  {
    cout << game.getMessages().getMessage("I_" + cardId);
    if (cardId == "BTLCY")
    {
      vector<Zone *> ownZones = {
        &game.getBoard().attackerBattle(),
        &game.getBoard().attackerField()
      };
      if (player.getId() == 2)
      {
        ownZones = {
          &game.getBoard().defenderBattle(),
          &game.getBoard().defenderField()
        };
      }
      for (Zone *zone: ownZones)
      {
        for (int i = 0; i < 7; ++i)
        {
          Card *c = zone->getCard(i);
          if (c && c->getType() == CardType::Creature)
          {
            CreatureCard *creature = dynamic_cast<CreatureCard *>(c);
            if (creature)
            {
              creature->addTrait(Trait::Haste);
              creature->addTrait(Trait::Temporary);
              creature->increaseAttack(3);
            }
          }
        }
      }
    }
    else if (cardId == "METOR")
    {
      vector<Zone *> zones = {
        &game.getBoard().attackerBattle(), &game.getBoard().defenderBattle(),
        &game.getBoard().attackerField(), &game.getBoard().defenderField()
      };
      for (Zone *zone: zones)
      {
        for (int i = 0; i < 7; ++i)
        {
          Card *c = zone->getCard(i);
          if (c && c->getType() == CardType::Creature)
          {
            CreatureCard *creature = dynamic_cast<CreatureCard *>(c);
            if (creature)
            {
              creature->takeDamage(3);
              if (creature->getHealth() <= 0)
              {
                shared_ptr<Card> dead = zone->extractCard(i);
                int ownerId = (zone == &game.getBoard().attackerBattle() || zone == &game.getBoard().attackerField())
                                ? 1
                                : 2;
                game.getPlayerById(ownerId).addToGraveyard(std::dynamic_pointer_cast<CreatureCard>(dead));
              }
            }
          }
        }
      }
    }
    else if (cardId == "FIRBL")
    {
      Player &enemy = game.getOpponentPlayer();
      int opponentId = enemy.getId();
      vector<Zone *> zones = {
        (opponentId == 1) ? &game.getBoard().attackerBattle() : &game.getBoard().defenderBattle(),
        (opponentId == 1) ? &game.getBoard().attackerField() : &game.getBoard().defenderField()
      };
      for (Zone *zone: zones)
      {
        for (int i = 0; i < 7; ++i)
        {
          Card *c = zone->getCard(i);
          if (c && c->getType() == CardType::Creature)
          {
            CreatureCard *creature = dynamic_cast<CreatureCard *>(c);
            if (creature)
            {
              creature->takeDamage(2);
              if (creature->getHealth() <= 0)
              {
                shared_ptr<Card> dead = zone->extractCard(i);
                int ownerId = (zone == &game.getBoard().attackerBattle() || zone == &game.getBoard().attackerField())
                                ? 1
                                : 2;
                game.getPlayerById(ownerId).addToGraveyard(std::dynamic_pointer_cast<CreatureCard>(dead));
              }
            }
          }
        }
      }
    }

    player.removeCardFromHand(card);
    player.subtractMana(manaCost);
    player.disableRedraw();
    return true;
  }
  // Target spells
  else if (type == SpellType::Target)
  {
    if (parts.size() != 3)
    {
      cout << game.getMessages().getMessage("E_INVALID_PARAM_COUNT_SPELL");
      return true;
    }
    string slot = parts[2];
    transform(slot.begin(), slot.end(), slot.begin(), ::toupper);
    if (!regex_match(slot, regex("O?(F|B)[1-7]")))
    {
      cout << game.getMessages().getMessage("E_INVALID_SLOT_SPELL");
      return true;
    }
    Zone *zone = nullptr;
    if (slot[0] == 'O')
    {
      int opponentId = game.getOpponentPlayer().getId();
      zone = (slot[1] == 'F')
               ? ((opponentId == 1) ? &game.getBoard().attackerField() : &game.getBoard().defenderField())
               : ((opponentId == 1) ? &game.getBoard().attackerBattle() : &game.getBoard().defenderBattle());
    }
    else
    {
      int currentId = game.getCurrentPlayer().getId();
      zone = (slot[0] == 'F')
               ? ((currentId == 1) ? &game.getBoard().attackerField() : &game.getBoard().defenderField())
               : ((currentId == 1) ? &game.getBoard().attackerBattle() : &game.getBoard().defenderBattle());
    }
    int index = slot.back() - '1';
    Card *target = zone->getCard(index);
    if (!target || target->getType() != CardType::Creature)
    {
      cout << game.getMessages().getMessage("E_TARGET_EMPTY");
      return true;
    }
    CreatureCard *creature = dynamic_cast<CreatureCard *>(target);

    int manaCost = spell->getManaCost();
    if (manaCost == -1)
    {
      if (cardId == "CURSE") manaCost = creature->getManaCost() + 1;
      else if (cardId == "CLONE") manaCost = (creature->getManaCost() + 1) / 2;
    }
    if (manaCost > game.getCurrentPlayer().getMana())
    {
      cout << game.getMessages().getMessage("E_NOT_ENOUGH_MANA");
      return true;
    }
    if (cardId == "SHOCK")
    {
      creature->takeDamage(1);
      if (creature->getHealth() <= 0)
      {
        std::shared_ptr<Card> removed = zone->extractCard(index);
        if (slot[0] == 'O') game.getOpponentPlayer().addToGraveyard(std::dynamic_pointer_cast<CreatureCard>(removed));
        else game.getCurrentPlayer().addToGraveyard(std::dynamic_pointer_cast<CreatureCard>(removed));
      }
    }
    else if (cardId == "MOBLZ")
    {
      creature->addTrait(Trait::Haste);
      creature->increaseAttack(1);
    }
    else if (cardId == "RRUSH")
    {
      creature->addTrait(Trait::FirstStrike);
      creature->addTrait(Trait::Temporary);
      creature->increaseAttack(2);
    }
    else if (cardId == "SHILD")
    {
      creature->increaseHealth(2);
    }
    else if (cardId == "AMPUT")
    {
      creature->removeFirstTraitAlphabetically();
    }
    else if (cardId == "FINAL")
    {
      creature->addTrait(Trait::Brutal);
      creature->addTrait(Trait::Haste);
      creature->addTrait(Trait::Temporary);
      creature->increaseAttack(3);
    }
    else if (cardId == "LYLTY")
    {
      creature->addTrait(Trait::Haste);
      creature->increaseHealth(1);
    }
    else if (cardId == "ZMBFY")
    {
      creature->addTrait(Trait::Venomous);
      creature->addTrait(Trait::Undying);
    }
    else if (cardId == "BLOOD")
    {
      creature->addTrait(Trait::Brutal);
      creature->addTrait(Trait::Lifesteal);
      int newHP = (creature->getHealth() + 1) / 2;
      creature->decreaseHealth(creature->getHealth() - newHP);
    }
    else if (cardId == "CURSE")
    {
      creature->addTrait(Trait::Temporary);
    }
    else if (cardId == "CLONE")
    {
      Zone &fieldZone = (player.getId() == 1)
                          ? game.getBoard().attackerField()
                          : game.getBoard().defenderField();
      int emptyIndex = -1;
      for (int i = 0; i < 7; ++i)
      {
        if (fieldZone.getCard(i) == nullptr)
        {
          emptyIndex = i;
          break;
        }
      }
      if (emptyIndex != -1)
      {
        shared_ptr<Card> clonedCard = game.getCardFactory().createCardByID(creature->getID());
        CreatureCard *clone = dynamic_cast<CreatureCard *>(clonedCard.get());
        clone->resetStats();
        clone->setSummonedRound(game.getCurrentRound());
        clone->increaseAttack(creature->getAttack() - clone->getAttack());
        clone->increaseHealth(creature->getHealth() - clone->getHealth());
        for (Trait t: creature->getBaseTraits())
        {
          if (!clone->hasTrait(t)) clone->addTrait(t);
        }
        clone->addTrait(Trait::Haste);
        clone->addTrait(Trait::Temporary);
        fieldZone.addCard(emptyIndex, clonedCard);
      }
    }
    player.removeCardFromHand(card);
    player.subtractMana(manaCost);
    player.disableRedraw();
    cout << game.getMessages().getMessage("I_" + cardId);
    return true;
  }
  // Graveyard spells
  else if (type == SpellType::Graveyard)
  {
    if (parts.size() != 3)
    {
      cout << game.getMessages().getMessage("E_INVALID_PARAM_COUNT_SPELL");
      return true;
    }
    string graveId = parts[2];
    transform(graveId.begin(), graveId.end(), graveId.begin(), ::toupper);
    const auto &grave = player.getGraveyard();
    auto it = std::find_if(grave.rbegin(), grave.rend(),
                           [&](const std::shared_ptr<CreatureCard> &c) { return c->getID() == graveId; });
    if (it == grave.rend())
    {
      cout << game.getMessages().getMessage("E_NOT_IN_GRAVEYARD");
      return true;
    }
    std::shared_ptr<CreatureCard> graveCreature = *it;
    if (cardId == "MEMRY")
    {
      int manaCost = (graveCreature->getManaCost() + 1) / 2;
      if (manaCost > player.getMana())
      {
        cout << game.getMessages().getMessage("E_NOT_ENOUGH_MANA");
        return true;
      }
      Zone &playerField = (player.getId() == 1)
                            ? game.getBoard().attackerField()
                            : game.getBoard().defenderField();
      int emptyIndex = -1;
      for (int i = 0; i < 7; ++i)
      {
        if (!playerField.getCard(i))
        {
          emptyIndex = i;
          break;
        }
      }
      if (emptyIndex != -1)
      {
        shared_ptr<Card> revived = game.getCardFactory().createCardByID(graveCreature->getID());
        CreatureCard *revivedCreature = dynamic_cast<CreatureCard *>(revived.get());
        revivedCreature->resetStats();
        revivedCreature->addTrait(Trait::Haste);
        revivedCreature->addTrait(Trait::Temporary);
        playerField.addCard(emptyIndex, revived);
      }
      player.removeCardFromHand(card);
      player.subtractMana(manaCost);
      player.disableRedraw();
      cout << game.getMessages().getMessage("I_MEMRY");
    }
    else if (cardId == "REVIV")
    {
      int manaCost = 2;
      if (manaCost > player.getMana())
      {
        cout << game.getMessages().getMessage("E_NOT_ENOUGH_MANA");
        return true;
      }

      auto &graveyard = player.getMutableGraveyard();
      auto it2 = std::find(graveyard.rbegin(), graveyard.rend(), graveCreature);
      if (it2 != graveyard.rend())
      {
        graveyard.erase(std::next(it2).base());
      }

      shared_ptr<Card> revived = game.getCardFactory().createCardByID(graveCreature->getID());
      CreatureCard *revivedCreature = dynamic_cast<CreatureCard *>(revived.get());
      revivedCreature->resetStats();
      player.addCardToHand(revived);

      player.removeCardFromHand(card);
      player.subtractMana(manaCost);
      player.disableRedraw();
      cout << game.getMessages().getMessage("I_REVIV");
    }
    return true;
  }
  return true;
}

// Prints unknown command error
bool CommandHandler::printUnknownCommand(const std::string & /*input*/, Game &game)
{
  std::cout << game.getMessages().getMessage("E_UNKNOWN_COMMAND");
  return true;
}
