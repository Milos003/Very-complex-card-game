// --------------------------- Deck.hpp ---------------------------
//
// This file declares the Deck class, which stores and manages a player's
// deck of cards. It supports loading cards from a factory, drawing cards,
// and tracking the remaining number of cards.
//
// Group: 051
//
// Author: Miloš Đukarić, Florian Kerman, Stefan Jović
// ------------------------------------------------------------------------
#pragma once

#include <vector>
#include <memory>
#include <string>
#include "Card.hpp"

class CardFactory;

// --------------------------------------------------------------------------
// Deck: Represents a deck of cards with draw functionality.
// --------------------------------------------------------------------------
class Deck
{
public:
  // ----------------------------------------------------------------------
  // Default constructor. Initializes an empty deck.
  // ----------------------------------------------------------------------
  Deck() = default;

  // ----------------------------------------------------------------------
  // Loads a sequence of card IDs into the deck using the CardFactory.
  // Resets internal draw pointer to the start.
  //
  // @param cardIDs Vector of card ID strings
  // @param factory Reference to CardFactory for creating card instances
  // ----------------------------------------------------------------------
  void loadFromIDs(const std::vector<std::string> &cardIDs, CardFactory &factory);

  // ----------------------------------------------------------------------
  // Draws the next available card from the deck.
  //
  // @return shared_ptr<Card> if available, nullptr if deck is empty
  // ----------------------------------------------------------------------
  std::shared_ptr<Card> draw();

  // ----------------------------------------------------------------------
  // Checks whether the deck is empty (no more cards to draw).
  //
  // @return true if empty, false otherwise
  // ----------------------------------------------------------------------
  bool isEmpty() const;

  // ----------------------------------------------------------------------
  // Returns the number of cards remaining to be drawn.
  //
  // @return size_t count of remaining cards
  // ----------------------------------------------------------------------
  size_t remaining() const;

  // ----------------------------------------------------------------------
  // Provides read-only access to all loaded cards (used for Player setup).
  //
  // @return const reference to vector of shared_ptr<Card>
  // ----------------------------------------------------------------------
  const std::vector<std::shared_ptr<Card> > &getCards() const;

private:
  std::vector<std::shared_ptr<Card> > cards;
  size_t currentIndex = 0;
};
