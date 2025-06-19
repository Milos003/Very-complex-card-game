// --------------------------- Deck.cpp ---------------------------
//
// This file implements the Deck class, which loads, stores and draws
// cards from a predefined sequence using a CardFactory for instantitation.
//
// Group: 051
//
// Author: <Miloš Đukarić, Florian Kerman, Stefan Jović>
// ------------------------------------------------------------------------
#include "Deck.hpp"
#include "CardFactory.hpp"
#include <memory>

using namespace std; // bring std names into scope for clarity

// --------------------------------------------------------------------------
// Loads a sequence of card IDs into the deck by querying the factory.
// Clears any existing cards, then adds valid ones.
// Resets the draw pointer to the top.
//
// @param cardIDs Vector of string card IDs to load
// @param factory CardFactory used to instantiate cards
// --------------------------------------------------------------------------
void Deck::loadFromIDs(const vector<string> &cardIDs, CardFactory &factory)
{
  cards.clear(); // remove previous contents
  for (const auto &id: cardIDs)
  {
    auto card = factory.createCardByID(id);
    if (card)
    {
      cards.push_back(card);
    }
    // if factory returns nullptr, skip invalid ID
  }
  currentIndex = 0; // reset draw pointer to top of deck
}

// --------------------------------------------------------------------------
// Draws the next card from the deck if available.
//
// @return shared_ptr to the drawn card, or nullptr if deck is empty
// --------------------------------------------------------------------------
shared_ptr<Card> Deck::draw()
{
  if (isEmpty())
  {
    return nullptr;
  }
  return cards[currentIndex++];
}

// --------------------------------------------------------------------------
// Checks if the deck is empty (no more cards to draw).
//
// @return true if empty, false otherwise
// --------------------------------------------------------------------------
bool Deck::isEmpty() const
{
  return currentIndex >= cards.size();
}

// --------------------------------------------------------------------------
// Returns how many cards remain in the deck to be drawn.
//
// @return Number of remaining cards
// --------------------------------------------------------------------------
size_t Deck::remaining() const
{
  return cards.size() - currentIndex;
}

// --------------------------------------------------------------------------
// Provides read-only access to the full card list.
//
// @return const reference to the vector of card pointers
// --------------------------------------------------------------------------
const vector<shared_ptr<Card> > &Deck::getCards() const
{
  return cards;
}
