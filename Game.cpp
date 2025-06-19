// --------------------------- Game.cpp ---------------------------
//
// Implements the main game logic, including round transitions
// battle phase resoulution and player command handling.
//
// Group: 051
//
// Author: <Miloš Đukarić, Florian Kerman, Stefan Jović>
// ------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include "CommandHandler.hpp"
#include "Game.hpp"

using namespace std;

//---------------------------------------------------------------------------------------------------------------------
///
/// Constructs the Game object by loading configuration files and initializing the game state.
///
/// This constructor loads all card definitions using the CardFactory, initializes the draw decks for
/// both players based on the configuration, assigns the decks to the players, and deals the initial
/// hand of cards to each player. It also determines the initial roles (attacker/defender) based on
/// round number.
///
/// @param gameConfigPath Path to the game configuration file (cards, decks, rounds, mana, etc.)
/// @param messageConfigPath Path to the message configuration file used for printed messages
//---------------------------------------------------------------------------------------------------------------------
Game::Game(const string &gameConfigPath, const string &messageConfigPath)
  : cfg(gameConfigPath),
    msgs(messageConfigPath),
    factory(),
    deck1(),
    deck2(),
    p1(1, cfg.getPlayerHealth(), cfg.getManaPoolStart()),
    p2(2, cfg.getPlayerHealth(), cfg.getManaPoolStart()),
    board(),
    roundNumber(1),
    currentPhase(Phase::Setup),
    boardPrinting(true),
    currentPlayerId(1),
    attacker(nullptr),
    defender(nullptr),
    result(GameResult::None),
    gameConfigPath(gameConfigPath)
{
  // 1) Load all creature and spell card definitions
  factory.loadCreatureCards();
  factory.loadSpellCards();

  // 2) Initialize each player's draw deck from config
  deck1.loadFromIDs(cfg.getPlayer1Deck(), factory);
  deck2.loadFromIDs(cfg.getPlayer2Deck(), factory);

  // 3) Assign decks to player objects and draw initial 6 cards
  p1.setDeck(deck1);
  p2.setDeck(deck2);
  p1.drawMultiple(7);
  p2.drawMultiple(7);

  updateRolesForNewRound();
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Runs the main game loop for Milestone 1.
///
/// Prints the welcome message and initial board state. If the game is not already over due to
/// configuration-based victory conditions (e.g. deck exhausted), it enters the player command prompt loop.
/// After the loop ends or the game is concluded, the result is written back to the config file.
///
/// @return int Exit status (0 = normal termination)
//---------------------------------------------------------------------------------------------------------------------
int Game::run()
{
  printWelcome(); // Welcome banner
  printBoard(); // Round header + board
  if (!isGameOver())
  {
    promptPlayer(); // Only enter loop if game is still running
  }

  writeResultToConfig();
  return 0;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Returns the attacker player reference.
///
/// @return const reference to the current attacker
//---------------------------------------------------------------------------------------------------------------------
const Player &Game::getAttacker() const
{
  return *attacker;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Returns the defender player reference.
///
/// @return const reference to the current defender
//---------------------------------------------------------------------------------------------------------------------
const Player &Game::getDefender() const
{
  return *defender;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Returns the player whose turn it currently is.
///
/// @return const reference to the current player
//---------------------------------------------------------------------------------------------------------------------
const Player &Game::getCurrentPlayer() const
{
  return (currentPlayerId == 1) ? p1 : p2;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Prints the welcome message using the message configuration.
/// Includes framed border and round header.
//---------------------------------------------------------------------------------------------------------------------
void Game::printWelcome()
{
  cout << msgs.getMessage("D_BORDER_D");
  cout << msgs.getMessage("D_WELCOME");
  cout << msgs.getMessage("D_BORDER_D");
  printRoundHeader();
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Prints the board with Player 1 positioned at the bottom.
//---------------------------------------------------------------------------------------------------------------------
void Game::printBoardWithP1OnBottom()
{
  board.print(roundNumber); // P1 is attacker (bottom)
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Prints the board with Player 2 positioned at the bottom.
//---------------------------------------------------------------------------------------------------------------------
void Game::printBoardWithP2OnBottom()
{
  board.print(roundNumber); // P2 is attacker (bottom)
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Initializes the round, printing the board according to current round parity.
//---------------------------------------------------------------------------------------------------------------------
void Game::initRound()
{
  // Only print at the start of the round
  if (boardPrinting)
  {
    if (roundNumber == 1 || roundNumber == 4 || roundNumber == 5 ||
        roundNumber == 8 || roundNumber == 9 || roundNumber == 12 ||
        roundNumber == 13 || roundNumber == 16 || roundNumber == 17 ||
        roundNumber == 20 || roundNumber == 21 || roundNumber == 24)
    {
      printBoardWithP2OnBottom(); // P1 is attacker in round 1 (bottom)
    }
    else
    {
      printBoardWithP1OnBottom(); // P2 attacker (bottom) in 2,3,6,7,...
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Sets the current active player.
///
/// @param id ID of the player to set as current
//---------------------------------------------------------------------------------------------------------------------
void Game::setCurrentPlayer(int id)
{
  currentPlayerId = id;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Prints the ASCII round header from messages.
//---------------------------------------------------------------------------------------------------------------------
void Game::printRoundHeader()
{
  cout << "\n";
  cout << msgs.getMessage("D_BORDER_D");
  // Centered label for current round number
  cout << "                                         ROUND " << roundNumber
      << "\n";
  cout << msgs.getMessage("D_BORDER_D");
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Begins the command input loop for the current player if the game is not yet over.
//---------------------------------------------------------------------------------------------------------------------
void Game::promptPlayer()
{
  string input;

  // TODO: Add all of the bollow here
  // There are three ways the game can end.
  // [ ] 1. A player has been reduced to zero health points or below. This
  // player loses, the other player wins. If both players' health points are
  // reduced to equal or smaller than zero in the same fight, the game ends in a
  // tie. [x] 2. A player needs to draw a card, but has no more cards to draw in
  // their deck. This player loses, the other player wins. [ ] 3. `<MAX_ROUNDS>`
  // has been exceeded. The player with the highest health points left is the
  // winner. If both players have the same amount of health points, the game
  // ends in a tie.
  //
  if (p1.getDeckRemaining() == 0 || p2.getDeckRemaining() == 0)
  {
    cout << endl;
    cout << msgs.getMessage("D_BORDER_GAME_END");
    cout << msgs.getMessage("D_END_DRAW_CARD");
    if (p1.getDeckRemaining() == 0)
    {
      cout << "Player 2 has won! Congratulations!\n";
      result = GameResult::P2_Wins;
    }
    else
    {
      cout << "Player 1 has won! Congratulations!\n";
      result = GameResult::P1_Wins;
    }
    cout << msgs.getMessage("D_BORDER_D");
    return;
  }

  while (!isGameOver())
  {
    cout << "\nP" << getCurrentPlayer().getId() << "> ";
    if (!getline(cin, input))
    {
      break;
    }

    // Only exit the loop if CommandHandler::process returns false
    if (!CommandHandler::process(input, *this))
    {
      break;
    }
    if (isGameOver())
    {
      break;
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Increments round, checks end conditions, draws cards, and updates mana.
//---------------------------------------------------------------------------------------------------------------------
void Game::incrementRound()
{
  roundNumber++;

  if (roundNumber > cfg.getMaxRounds())
  {
    cout << "\n" << msgs.getMessage("D_BORDER_GAME_END");
    cout << msgs.getMessage("D_END_MAX_ROUNDS");

    int hp1 = p1.getHealth();
    int hp2 = p2.getHealth();

    if (hp1 > hp2) result = GameResult::P1_Wins;
    else if (hp2 > hp1) result = GameResult::P2_Wins;
    else result = GameResult::Tie;

    if (hp1 > hp2)
    {
      cout << "Player 1 has won! Congratulations!\n";
    }
    else if (hp2 > hp1)
    {
      cout << "Player 2 has won! Congratulations!\n";
    }
    else
    {
      cout << msgs.getMessage("D_TIE");
    }

    cout << msgs.getMessage("D_BORDER_D");
    endGame();
    return;
  }
  if (p1.getDeckRemaining() == 0 || p2.getDeckRemaining() == 0)
  {
    printRoundHeader();
    cout << "\n" << msgs.getMessage("D_BORDER_GAME_END");
    cout << msgs.getMessage("D_END_DRAW_CARD");

    int currentId = getCurrentPlayer().getId();

    if (getCurrentPlayer().getDeckRemaining() == 0)
    {
      cout << "Player " << getOpponentPlayer().getId() << " has won! Congratulations!" << endl;
    }
    else
    {
      cout << "Player " << currentId << " has won! Congratulations!" << endl;
    }

    cout << msgs.getMessage("D_BORDER_D");
    endGame();
    return;
  }

  p1.drawCard();
  p2.drawCard();

  if (roundNumber % 2 == 1)
  {
    p1.setManaPoolSize(p1.getManaPoolSize() + 1);
    p2.setManaPoolSize(p2.getManaPoolSize() + 1);

    p1.setMana(p1.getManaPoolSize());
    p2.setMana(p2.getManaPoolSize());
  } /*increases Mana in each odd round*/
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Updates the attacker and defender roles depending on the current round number.
//--------------------------------------------------------------------------------------------------------------------
void Game::updateRolesForNewRound()
{
  if (roundNumber == 1 || roundNumber == 4 || roundNumber == 5 ||
      roundNumber == 8 || roundNumber == 9 || roundNumber == 12 ||
      roundNumber == 13 || roundNumber == 16 || roundNumber == 17 ||
      roundNumber == 20 || roundNumber == 21 || roundNumber == 24)
  {
    setAttacker(p1);
    setDefender(p2);
    setCurrentPlayer(1);
  }
  else
  {
    setAttacker(p2);
    setDefender(p1);
    setCurrentPlayer(2);
  }
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Prints the board for the current round, flipping layout for even-numbered rounds.
//---------------------------------------------------------------------------------------------------------------------
void Game::printBoard()
{
  if (!isBoardPrinting())
  {
    return;
  }
  // Flip layout only on even-numbered rounds
  if (roundNumber % 2 == 0)
  {
    board.print(roundNumber); // P1 on top
  }
  else
  {
    board.print(roundNumber); // P2 on top
  }
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Handles direct damage to the defender when no defending card is present.
///
/// @param damage Amount of damage to apply
/// @return true if the game is over, false otherwise
//---------------------------------------------------------------------------------------------------------------------
bool Game::handleDirectHit(int damage)
{
  defender->setHealth(defender->getHealth() - damage);
  cout << msgs.getMessage("I_DIRECT");

  if (defender->getHealth() <= 0)
  {
    if (attacker->getId() == 1)
    {
      result = GameResult::P1_Wins;
    }
    else
    {
      result = GameResult::P2_Wins;
    }

    cout << "\n" << msgs.getMessage("D_BORDER_GAME_END");
    cout << msgs.getMessage("D_END_PLAYER_DEFEATED");
    cout << "Player " << attacker->getId() << " has won! Congratulations!\n";
    cout << msgs.getMessage("D_BORDER_D");
    endGame();

    return true; // game is over
  }
  return false; // continue processing
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Handles direct damage to the attacker when no attacking card is present.
///
/// @param damage Amount of damage to apply
/// @return true if the game is over, false otherwise
//---------------------------------------------------------------------------------------------------------------------
bool Game::handleDirectHitToAttacker(int damage)
{
  attacker->setHealth(attacker->getHealth() - damage);
  cout << msgs.getMessage("I_DIRECT");

  if (attacker->getHealth() <= 0)
  {
    if (defender->getId() == 1)
    {
      result = GameResult::P1_Wins;
    }
    else
    {
      result = GameResult::P2_Wins;
    }

    cout << "\n" << msgs.getMessage("D_BORDER_GAME_END");
    cout << msgs.getMessage("D_END_PLAYER_DEFEATED");
    cout << "Player " << defender->getId() << " has won! Congratulations!\n";
    cout << msgs.getMessage("D_BORDER_D");
    endGame();

    return true; // game is over
  }
  return false; // continue processing
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Checks whether the current player is also the defender.
///
/// @return true if current player is defender, false otherwise
//---------------------------------------------------------------------------------------------------------------------
bool Game::currentPlayerIsDefender() const
{
  return &getCurrentPlayer() == defender;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Returns a reference to the player object with given ID.
///
/// @param id Player ID (1 or 2)
/// @return reference to corresponding Player object
//---------------------------------------------------------------------------------------------------------------------
Player &Game::getPlayerById(int id)
{
  return (id == 1) ? p1 : p2;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Processes the entire battle phase logic including traits and damage resolution.
//---------------------------------------------------------------------------------------------------------------------
void Game::processBattlePhase()
{
  cout << "\n" << msgs.getMessage("D_BORDER_BATTLE_PHASE");

  Zone &attackerBattle = (attacker->getId() == 1)
                           ? board.attackerBattle()
                           : board.defenderBattle();
  Zone &defenderBattle = (defender->getId() == 1)
                           ? board.attackerBattle()
                           : board.defenderBattle();

  /*checks if the game should end*/
  bool gameShouldEnd = false;
  int winnerId = 0;

  /* handles battle phase, prints out slots*/
  for (int i = 0; i < 7; ++i)
  {
    if (p1.getHealth() <= 0 && p2.getHealth() <= 0)
    {
      cout << "\n" << msgs.getMessage("D_BORDER_GAME_END");
      cout << msgs.getMessage("D_END_PLAYER_DEFEATED");
      cout << msgs.getMessage("D_TIE");
      cout << msgs.getMessage("D_BORDER_D");
      endGame();
      return;
    }

    if (isGameOver())
    {
      return;
    }

    cout << "---------------------------------------- SLOT " << (i + 1)
        << " -----------------------------------------" << endl;


    Card *atkCard = attackerBattle.getCard(i);
    Card *defCard = defenderBattle.getCard(i);

    // direct hits
    if (!atkCard || atkCard->getType() != CardType::Creature)
    {
      Card *attackerFieldCard = (attacker->getId() == 1)
                                  ? board.attackerField().getCard(i)
                                  : board.defenderField().getCard(i);

      if (attackerFieldCard)
      {
        continue;
      }

      if (defCard && defCard->getType() == CardType::Creature)
      {
        CreatureCard *defenderCreature = dynamic_cast<CreatureCard *>(defCard);
        if (defenderCreature)
        {
          if (handleDirectHitToAttacker(defenderCreature->getAttack()))
          {
            return;
          }
        }
      }

      continue;
    }

    if (!defCard || defCard->getType() != CardType::Creature)
    {
      CreatureCard *attackerCreature = dynamic_cast<CreatureCard *>(atkCard);
      if (attackerCreature && handleDirectHit(attackerCreature->getAttack()))
      {
        return;
      }
      continue;
    }

    // if everything is okay iit goes to the battle
    CreatureCard *attackerCreature = static_cast<CreatureCard *>(atkCard);

    /*Attack 1 starts */
    CreatureCard *defenderCreature = static_cast<CreatureCard *>(defCard);
    cout << msgs.getMessage("I_FIGHT");

    int defBeforeHP = defenderCreature->getHealth();

    /* sets attacker damage and defender damage*/
    int attackerDamage = attackerCreature->getAttack();
    int defenderDamage = defenderCreature->getAttack();

    bool attackerBrutal = attackerCreature->hasTrait(Trait::Brutal);
    bool defenderBrutal = defenderCreature->hasTrait(Trait::Brutal);

    bool attackerFirstStrike = attackerCreature->hasTrait(Trait::FirstStrike);
    bool defenderFirstStrike = defenderCreature->hasTrait(Trait::FirstStrike);

    /* First strike handling */
    if (attackerFirstStrike ^ defenderFirstStrike)
    {
      cout << msgs.getMessage("D_ATTACK_1");
      cout << msgs.getMessage("I_FIRST_STRIKE");

      if (attackerFirstStrike)
      {
        defenderCreature->takeDamage(attackerDamage);

        if (attackerBrutal && defenderCreature->getHealth() <= 0)
        {
          int overkill = attackerDamage - defBeforeHP;
          if (overkill > 0)
          {
            defender->setHealth(defender->getHealth() - overkill);
          }
        }

        if (attackerBrutal)
        {
          attackerDamage += 1;
          cout << msgs.getMessage("I_BRUTAL");
        }

        if (defenderCreature->getHealth() <= 0)
        {
          continue;
        }

        /* posioned trait */
        if (defenderCreature->getHealth() > 0 && attackerCreature->hasTrait(Trait::Poisoned))
        {
          defenderCreature->addTrait(Trait::Poisoned);
          cout << msgs.getMessage("I_POISONED") << endl;
        }

        /* venomous trait */
        if (defenderCreature->getHealth() > 0 && attackerCreature->hasTrait(Trait::Venomous))
        {
          defenderCreature->addTrait(Trait::Poisoned);
          cout << msgs.getMessage("I_VENOMOUS");
        }

        /* lifesteal handling */
        if (attackerCreature->hasTrait(Trait::Lifesteal))
        {
          attacker->setHealth(attacker->getHealth() + attackerDamage);
          cout << msgs.getMessage("I_LIFESTEAL");
        }

        /* --- ATTACK 2 BEGINS */
        int attBeforeHP = attackerCreature->getHealth();

        if (defenderCreature->getHealth() > 0)
        {
          cout << msgs.getMessage("D_ATTACK_2");

          attackerCreature->takeDamage(defenderDamage);

          if (defenderBrutal && attackerCreature->getHealth() <= 0)
          {
            int overkill = defenderDamage - attBeforeHP;
            if (overkill > 0)
            {
              attacker->setHealth(attacker->getHealth() - overkill);
            }
          }

          if (defenderBrutal)
          {
            defenderDamage += 1;
            cout << msgs.getMessage("I_BRUTAL");
          }

          if (attackerCreature->getHealth() <= 0)
          {
            continue;
          }

          /* poisioned trait */
          if (attackerCreature->getHealth() > 0 && defenderCreature->hasTrait(Trait::Poisoned))
          {
            attackerCreature->addTrait(Trait::Poisoned);
            cout << msgs.getMessage("I_POISONED") << endl;
          }

          /* venomous trait */
          if (attackerCreature->getHealth() > 0 && defenderCreature->hasTrait(Trait::Venomous))
          {
            attackerCreature->addTrait(Trait::Poisoned);
            cout << msgs.getMessage("I_VENOMOUS");
          }

          /* lifesteal handling*/
          if (defenderCreature->hasTrait(Trait::Lifesteal))
          {
            defender->setHealth(defender->getHealth() + defenderDamage);
            cout << msgs.getMessage("I_LIFESTEAL");
          }
        }
      }
      else
      {
        attackerCreature->takeDamage(defenderDamage);

        /* handles overkill //needs to be improved */
        if (defenderBrutal && attackerCreature->getHealth() <= 0)
        {
          int overkill = defenderDamage - attackerCreature->getBaseHP();

          if (overkill > 0) attacker->setHealth(attacker->getHealth() - overkill);
        }
        if (defenderBrutal)
        {
          defenderDamage += 1;
          cout << msgs.getMessage("I_BRUTAL");
        }

        /* poisioned trait */
        if (attackerCreature->getHealth() > 0 &&
            defenderCreature->hasTrait(Trait::Poisoned))
        {
          attackerCreature->addTrait(Trait::Poisoned);
          cout << msgs.getMessage("I_POISONED") << endl;
        }

        /* venomous trait */
        if (attackerCreature->getHealth() > 0 &&
            defenderCreature->hasTrait(Trait::Venomous))
        {
          attackerCreature->addTrait(Trait::Poisoned);
          cout << msgs.getMessage("I_VENOMOUS");
        }

        /* lifesteal handling */
        if (defenderCreature->hasTrait(Trait::Lifesteal))
        {
          defender->setHealth(defender->getHealth() + defenderDamage);
          cout << msgs.getMessage("I_LIFESTEAL") << endl;
        }

        /* ----ATTACK 2 BEGINS ----*/
        if (attackerCreature->getHealth() > 0)
        {
          cout << msgs.getMessage("D_ATTACK_2");

          defenderCreature->takeDamage(attackerDamage);

          if (attackerBrutal && defenderCreature->getHealth() <= 0)
          {
            int overkill = attackerDamage - defenderCreature->getBaseHP();
            if (overkill > 0) defender->setHealth(defender->getHealth() - overkill);
          }

          if (attackerBrutal)
          {
            attackerDamage += 1;
            cout << msgs.getMessage("I_BRUTAL");
          }

          /* poisioned trait handling */
          if (defenderCreature->getHealth() > 0 &&
              attackerCreature->hasTrait(Trait::Poisoned))
          {
            defenderCreature->addTrait(Trait::Poisoned);
            cout << msgs.getMessage("I_POISONED") << endl;
          }
          /* venomous trait handling */
          if (defenderCreature->getHealth() > 0 &&
              attackerCreature->hasTrait(Trait::Venomous))
          {
            defenderCreature->addTrait(Trait::Poisoned);
            cout << msgs.getMessage("I_VENOMOUS");
          }

          /* lifesteal handling */
          if (attackerCreature->hasTrait(Trait::Lifesteal))
          {
            attacker->setHealth(attacker->getHealth() + attackerDamage);
            cout << msgs.getMessage("I_LIFESTEAL");
          }
        }
      }
      /* In case of a normal battle, nobody has a first strike */
    }
    else
    {
      // --- ATTACK 1 ---
      cout << msgs.getMessage("D_ATTACK_1");

      defenderCreature->takeDamage(attackerDamage);

      if (attackerBrutal && defenderCreature->getHealth() <= 0)
      {
        int overkill = attackerDamage - defenderCreature->getBaseHP();
        if (overkill > 0) defender->setHealth(defender->getHealth() - overkill);
      }

      if (attackerBrutal)
      {
        attackerDamage += 1;
        cout << msgs.getMessage("I_BRUTAL");
      }

      /* poisioned trait */
      if (defenderCreature->getHealth() > 0 &&
          attackerCreature->hasTrait(Trait::Poisoned))
      {
        defenderCreature->addTrait(Trait::Poisoned);
        cout << msgs.getMessage("I_POISONED") << endl;
      }

      /* venomous trait handling */
      if (defenderCreature->getHealth() > 0 &&
          attackerCreature->hasTrait(Trait::Venomous))
      {
        defenderCreature->addTrait(Trait::Poisoned);
        cout << msgs.getMessage("I_VENOMOUS");
      }

      /* lifesteal handling */
      if (attackerCreature->hasTrait(Trait::Lifesteal))
      {
        attacker->setHealth(attacker->getHealth() + attackerDamage);
        cout << msgs.getMessage("I_LIFESTEAL");
      }

      // --- ATTACK 2 ---
      cout << msgs.getMessage("D_ATTACK_2");

      attackerCreature->takeDamage(defenderDamage);

      if (defenderBrutal && attackerCreature->getHealth() <= 0)
      {
        int overkill = defenderDamage - attackerCreature->getBaseHP();
        if (overkill > 0) attacker->setHealth(attacker->getHealth() - overkill);
      }

      if (defenderBrutal)
      {
        defenderDamage += 1;
        cout << msgs.getMessage("I_BRUTAL");
      }

      /* posioned trait */
      if (attackerCreature->getHealth() > 0 &&
          defenderCreature->hasTrait(Trait::Poisoned))
      {
        attackerCreature->addTrait(Trait::Poisoned);
        cout << msgs.getMessage("I_POISONED") << endl;
      }

      /* venomous trait */
      if (defenderCreature->hasTrait(Trait::Venomous))
      {
        attackerCreature->addTrait(Trait::Poisoned);
        cout << msgs.getMessage("I_VENOMOUS");
      }

      /* lifesteal handling */
      if (defenderCreature->hasTrait(Trait::Lifesteal))
      {
        defender->setHealth(defender->getHealth() + defenderDamage);
        cout << msgs.getMessage("I_LIFESTEAL");
      }
    }
  }

  // Return creatures to their original field slots after battle
  auto returnBattleToFieldZone = [&](Zone &battleZone, Zone &fieldZone,
                                     Player *owner)
  {
    for (int i = 0; i < 7; ++i)
    {
      Card *rawCard = battleZone.getCard(i);
      CreatureCard *rawCreature = dynamic_cast<CreatureCard *>(rawCard);
      if (!rawCreature) continue;

      // Skip if creature is already resurrected (avoids duplicate processing)
      // if (rawCreature->isResurrected()) continue;


      shared_ptr<Card> movingCard = battleZone.extractCard(i);
      CreatureCard *creature = dynamic_cast<CreatureCard *>(movingCard.get());


      if (!creature) continue;

      // === Handle dead creatures ===
      if (creature->getHealth() <= 0)
      {
        if (creature->hasTrait(Trait::Regenerate))
        {
          creature->setHealth(creature->getBaseHP());
          cout << msgs.getMessage("I_REGENERATE");
        }
        else if (creature->hasTrait(Trait::Undying))
        {
          creature->resetStats(); // <== resets stats / health and so on
          creature->removeTrait(Trait::Undying); // <== removes the trait
          creature->markResurrected(); // <== prevent re-processing
          cout << msgs.getMessage("I_UNDYING");


          owner->removeFromGraveyard(std::dynamic_pointer_cast<CreatureCard>(movingCard));
          // === Try placing creature back to field immediately
          bool placed = false;
          for (int j = 0; j < 7; ++j)
          {
            if (!fieldZone.getCard(j))
            {
              fieldZone.addCard(j, movingCard);
              placed = true;
              break;
            }
          }

          if (!placed)
          {
            owner->addToGraveyard(std::dynamic_pointer_cast<CreatureCard>(movingCard));
            // <== if there is no place send it to the graveyard
          }

          continue; // skip rest of the loop
        }
        else
        {
          owner->addToGraveyard(std::dynamic_pointer_cast<CreatureCard>(movingCard));
          continue; // Skip placing on field
        }
      }

      // === Try placing on field
      bool placed = false;
      for (int j = 0; j < 7; ++j)
      {
        if (!fieldZone.getCard(j))
        {
          fieldZone.addCard(j, movingCard);
          placed = true;
          break;
        }
      }

      if (!placed)
      {
        owner->addToGraveyard(std::dynamic_pointer_cast<CreatureCard>(movingCard));
      }
    }
  };
  // cleans up temporary creatures from the board
  auto cleanUpTemporaryCreatures = [&](Zone &fieldZone, Player *player)
  {
    for (int i = 0; i < 7; ++i)
    {
      Card *card = fieldZone.getCard(i);
      CreatureCard *creature = dynamic_cast<CreatureCard *>(card);
      if (!creature) continue;
      if (creature->hasTrait(Trait::Temporary))
      {
        cout << msgs.getMessage("I_TEMPORARY");
        shared_ptr<Card> removed = fieldZone.extractCard(i);
        player->addToGraveyard(std::dynamic_pointer_cast<CreatureCard>(removed));
      }
    }
  };

  auto handleUndyingInGraveyard = [&](Player *player)
  {
    Zone &fieldZone =
        (player->getId() == 1) ? board.attackerField() : board.defenderField();
    std::vector<std::shared_ptr<CreatureCard> > resurrected;
    for (const auto &card: player->getGraveyard())
    {
      if (card->hasTrait(Trait::Undying))
      {
        cout << msgs.getMessage("I_UNDYING");
        card->resetStats();
        card->removeTrait(Trait::Undying);
        resurrected.push_back(card);
      }
    }

    for (const auto &card: resurrected)
    {
      bool placed = false;
      for (int j = 0; j < 7; ++j)
      {
        if (!fieldZone.getCard(j))
        {
          fieldZone.addCard(j, card);
          placed = true;
          break;
        }
      }

      if (placed)
      {
        getPlayerById(player->getId()).removeFromGraveyard(card);
      }
    }
  };


  Zone &field1 =
      (attacker->getId() == 1) ? board.attackerField() : board.defenderField();
  Zone &field2 =
      (defender->getId() == 1) ? board.attackerField() : board.defenderField();
  cout << msgs.getMessage("D_BORDER_BATTLE_END");


  returnBattleToFieldZone(attackerBattle, field1, attacker);
  returnBattleToFieldZone(defenderBattle, field2, defender);

  cleanUpTemporaryCreatures(field1, attacker);
  cleanUpTemporaryCreatures(field2, defender);

  handleUndyingInGraveyard(attacker);
  handleUndyingInGraveyard(defender);


  // in the case where game is over it prints out ending part
  if (gameShouldEnd)
  {
    cout << "\n" << msgs.getMessage("D_BORDER_GAME_END");
    if (winnerId == 0)
    {
      cout << msgs.getMessage("D_TIE");
    }
    else
    {
      cout << "Player " << winnerId << " has won! Congratulations!\n";
    }
    cout << msgs.getMessage("D_BORDER_D");
    endGame();
    return;
  }
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Appends the game result (win/loss/tie) to the original configuration file.
//---------------------------------------------------------------------------------------------------------------------
void Game::writeResultToConfig()
{
  std::ofstream out(gameConfigPath, std::ios::app);
  if (!out.is_open())
  {
    std::cout << "<I_FILE_WRITE_FAILED>\n";
    return;
  }

  out << "\n";
  switch (result)
  {
    case GameResult::P1_Wins: out << "Player 1 has won! Congratulations!";
      break;
    case GameResult::P2_Wins: out << "Player 2 has won! Congratulations!";
      break;
    case GameResult::Tie: out << "It's a tie...";
      break;
    default: break;
  }

  out << "\n";
  out.close();
}
