// --------------------------- ConfigParser.hpp ---------------------------
//
// This file declares the GameConfigParser class, which parses 
// game configuration files that define constants and player decks.
//
// Group: 051
//
// Author: Miloš Đukarić, Florian Kerman, Stefan Jović
// ------------------------------------------------------------------------
#pragma once

#include <string>
#include <vector>

// -------------------------------------------------------------
// GameConfigParser: parses the initial game configuration.
// -------------------------------------------------------------
class GameConfigParser
{
public:
  // -------------------------------------------------------------
  //
  // Constructor. Immediately attempts to parse the given config file.
  //
  // @param filePath Path to the game config file
  // @throws std::runtime_error on I/O or format error
  //
  // -------------------------------------------------------------
  GameConfigParser(const std::string &filePath);

  // -------------------------------------------------------------
  //
  // Getter methods for game constants defined in the config.
  //
  // @return Integer value for each respective config field
  //
  // -------------------------------------------------------------
  int getPlayerHealth() const;

  int getMaxRounds() const;

  int getDeckSize() const;

  int getManaPoolStart() const;

  // -------------------------------------------------------------
  //
  // Returns the parsed list of card IDs for Player 1 and Player 2.
  //
  // @return Reference to a vector of strings
  //
  // -------------------------------------------------------------
  const std::vector<std::string> &getPlayer1Deck() const;

  const std::vector<std::string> &getPlayer2Deck() const;

private:
  int playerHealth;
  int maxRounds;
  int deckSize;
  int manaPoolStart;
  std::vector<std::string> deck1;
  std::vector<std::string> deck2;

  // -------------------------------------------------------------
  //
  // Parses the game configuration file and sets member values.
  //
  // @param filePath Path to the file to parse
  // @throws std::runtime_error on invalid format
  //
  // -------------------------------------------------------------
  void parseFile(const std::string &filePath);

  // -------------------------------------------------------------
  //
  // Helper function to split semicolon-separated card IDs.
  //
  // @param line Line from config file (e.g. "ID1;ID2;ID3")
  // @return Vector of extracted card IDs
  //
  // -------------------------------------------------------------
  std::vector<std::string> splitIds(const std::string &line) const;
};
