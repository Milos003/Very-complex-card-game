// --------------------------- Player.cpp ---------------------------
//
// Implementation of the Player class for handling hand, deck, graveyard, mana, and card-related logic.
//
// Group: 051
//
// Author: Miloš Đukarić, Florian Kerman, Stefan Jović
// ------------------------------------------------------------------------
#include "Player.hpp"
#include "Deck.hpp"
#include "CreatureCard.hpp"
#include "Card.hpp"

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

//---------------------------------------------------------------------------------------------------------------------
///
/// Constructs a player object with given ID, starting health and mana pool size.
///
/// @param playerId The player's ID (1 or 2)
/// @param startHealth The initial health of the player
/// @param manaPoolStart The starting mana and mana pool size
///
//---------------------------------------------------------------------------------------------------------------------
Player::Player(int playerId, int startHealth, int manaPoolStart)
  : id(playerId)
    , health(startHealth)
    , mana(manaPoolStart)
    , manaPoolSize(manaPoolStart)
    , redrawEnabled(true)
    , graveyard()
{
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Sets the player's deck from an external Deck object (copies pointers).
///
/// @param d The Deck to copy cards from
///
//---------------------------------------------------------------------------------------------------------------------
void Player::setDeck(Deck &d)
{
  deck.clear();
  for (auto &ptr: d.getCards())
  {
    deck.push_back(ptr); // shared_ptr<Card>
  }
}

//---------------------------------------------------------------------------------------------------------------------
int Player::getId() const { return id; }
int Player::getHealth() const { return health; }
int Player::getMana() const { return mana; }
int Player::getManaPoolSize() const { return manaPoolSize; }
size_t Player::getDeckRemaining() const { return deck.size(); }
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
///
/// Draws a single card from the deck and adds it to the hand.
/// If the deck is empty, does nothing.
///
//---------------------------------------------------------------------------------------------------------------------
void Player::drawCard()
{
  if (!deck.empty())
  {
    std::shared_ptr<Card> top = deck.front();
    deck.erase(deck.begin());
    hand.push_back(top);
  }
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Draws multiple cards from the deck.
///
/// @param n The number of cards to draw
///
//---------------------------------------------------------------------------------------------------------------------
void Player::drawMultiple(int n)
{
  for (int i = 0; i < n; ++i)
  {
    drawCard();
  }
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Searches for a card in hand by ID (case-insensitive).
///
/// @param id The card ID to search for
///
/// @return Pointer to the card if found, nullptr otherwise
///
//---------------------------------------------------------------------------------------------------------------------
Card *Player::findCardInHandById(const std::string &id)
{
  std::string upperId = id;
  std::transform(upperId.begin(), upperId.end(), upperId.begin(), ::toupper);

  for (const auto &c: hand)
  {
    std::string cardId = c->getID();
    std::transform(cardId.begin(), cardId.end(), cardId.begin(), ::toupper);
    if (cardId == upperId)
    {
      return c.get();
    }
  }
  return nullptr;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Checks if player is eligible to redraw (hasn't used it and has at least 2 cards).
///
/// @return true if redraw is possible
///
//---------------------------------------------------------------------------------------------------------------------
bool Player::canRedraw() const
{
  return redrawEnabled && hand.size() >= 2;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Disables further use of redraw mechanic for this player.
///
//---------------------------------------------------------------------------------------------------------------------
void Player::disableRedraw()
{
  redrawEnabled = false;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Returns all cards from hand to bottom of the deck, clearing the hand.
///
//---------------------------------------------------------------------------------------------------------------------
void Player::returnHandToBottomOfDeck()
{
  for (auto &card: hand)
  {
    deck.push_back(card);
  }
  hand.clear();
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Performs the redraw: returns hand to deck, draws one less card.
///
/// Skips if redraw is not available.
///
//---------------------------------------------------------------------------------------------------------------------
void Player::performRedraw()
{
  if (!canRedraw()) return;

  int original = static_cast<int>(hand.size());
  returnHandToBottomOfDeck();
  drawMultiple(original - 1);
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Removes a specific card from the hand (pointer equality).
///
/// @param card Pointer to the card to remove
///
//---------------------------------------------------------------------------------------------------------------------
void Player::removeCardFromHand(Card *card)
{
  auto it = std::find_if(hand.begin(), hand.end(),
                         [card](const shared_ptr<Card> &ptr)
                         {
                           return ptr.get() == card;
                         });
  if (it != hand.end())
  {
    hand.erase(it);
  }
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Subtracts mana from the player's current mana pool.
///
/// @param amount Amount to subtract
///
//---------------------------------------------------------------------------------------------------------------------
void Player::subtractMana(int amount)
{
  mana -= amount;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Adds a creature card to the player's graveyard.
///
/// @param creature Pointer to the creature card
///
//---------------------------------------------------------------------------------------------------------------------
void Player::addToGraveyard(std::shared_ptr<CreatureCard> creature)
{
  graveyard.push_back(creature);
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Adds a card to the player's hand.
///
/// @param card Shared pointer to the card
///
//---------------------------------------------------------------------------------------------------------------------
void Player::addCardToHand(std::shared_ptr<Card> card)
{
  hand.push_back(std::move(card));
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Returns a reference to the graveyard (modifiable).
///
/// @return Reference to vector of creature pointers
///
//---------------------------------------------------------------------------------------------------------------------
std::vector<std::shared_ptr<CreatureCard> > &Player::getMutableGraveyard()
{
  return graveyard;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Prints the player's hand in ASCII visual style.
///
//---------------------------------------------------------------------------------------------------------------------
void Player::printHand() const
{
  const int maxPerRow = 7;

  for (size_t i = 0; i < hand.size(); i += maxPerRow)
  {
    size_t end = min(i + maxPerRow, hand.size());

    // Mana cost
    cout << "    ";
    for (size_t j = i; j < end; ++j)
    {
      int cost = hand[j]->getManaCost();
      string mc = (cost < 0) ? "XX" : (cost > 99 ? "**" : (cost < 10 ? "0" + to_string(cost) : to_string(cost)));
      cout << " _____M" << mc;
      if (j != end - 1) cout << "   ";
    }
    cout << "\n";

    // Card ID
    cout << "    ";
    for (size_t j = i; j < end; ++j)
    {
      cout << "| " << setw(5) << left << hand[j]->getID() << " |";
      if (j != end - 1) cout << "   ";
    }
    cout << "\n";

    // Traits or blank
    cout << "    ";
    for (size_t j = i; j < end; ++j)
    {
      if (hand[j]->getType() == CardType::Creature)
      {
        CreatureCard *c = dynamic_cast<CreatureCard *>(hand[j].get());
        string traits = c->getTraitsString();
        traits.resize(5, ' ');
        if (traits.size() > 5) traits = traits.substr(0, 4) + "+";
        cout << "| " << traits << " |";
      }
      else
      {
        cout << "|       |";
      }
      if (j != end - 1) cout << "   ";
    }
    cout << "\n";

    // Bottom line (ATK/HP or blanks)
    cout << "    ";
    for (size_t j = i; j < end; ++j)
    {
      if (hand[j]->getType() == CardType::Creature)
      {
        CreatureCard *c = dynamic_cast<CreatureCard *>(hand[j].get());
        string atk = (c->getAttack() > 99)
                       ? "**"
                       : (c->getAttack() < 10 ? "0" + to_string(c->getAttack()) : to_string(c->getAttack()));
        string hp = (c->getHealth() > 99)
                      ? "**"
                      : (c->getHealth() < 10 ? "0" + to_string(c->getHealth()) : to_string(c->getHealth()));
        cout << "A" << atk << "___H" << hp;
      }
      else
      {
        cout << " _______ ";
      }
      if (j != end - 1) cout << "   ";
    }
    cout << "\n";
  }
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Removes a creature card from the graveyard based on ID match.
///
/// @param card Pointer to the card to remove
///
//---------------------------------------------------------------------------------------------------------------------
void Player::removeFromGraveyard(std::shared_ptr<CreatureCard> card)
{
  graveyard.erase(std::remove_if(graveyard.begin(), graveyard.end(),
                                 [&](const std::shared_ptr<CreatureCard> &c)
                                 {
                                   return c->getID() == card->getID();
                                 }), graveyard.end());
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Sets the player's health, clamped to minimum 0.
///
/// @param newHealth The new health value
///
//---------------------------------------------------------------------------------------------------------------------
void Player::setHealth(int newHealth)
{
  health = max(0, newHealth);
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Sets current mana, clamped between 0 and mana pool size.
///
/// @param newMana New mana value
///
//---------------------------------------------------------------------------------------------------------------------
void Player::setMana(int newMana)
{
  mana = clamp(newMana, 0, manaPoolSize);
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Sets the maximum mana pool size (must be non-negative).
///
/// @param newSize New size of the mana pool
///
//---------------------------------------------------------------------------------------------------------------------
void Player::setManaPoolSize(int newSize)
{
  manaPoolSize = max(0, newSize);
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Returns the list of cards currently in the player's hand.
///
/// @return Const reference to hand vector
///
//---------------------------------------------------------------------------------------------------------------------
const vector<shared_ptr<Card> > &Player::getHand() const
{
  return hand;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Returns the list of creature cards in the player's graveyard.
///
/// @return Const reference to graveyard vector
///
//---------------------------------------------------------------------------------------------------------------------
const std::vector<std::shared_ptr<CreatureCard> > &Player::getGraveyard() const
{
  return graveyard;
}

std::shared_ptr<Card> Player::extractCardFromHand(Card *rawPtr)
{
  auto it = std::find_if(hand.begin(), hand.end(),
                         [rawPtr](const std::shared_ptr<Card> &c)
                         {
                           return c.get() == rawPtr;
                         });

  if (it != hand.end())
  {
    auto extracted = *it;
    hand.erase(it);
    return extracted;
  }
  return nullptr;
}
