// --------------------------- ConfigParser.cpp ---------------------------
//
// This file implements the GameConfigParser class, which loads and parses
// the configuration file that defines player health, round limits,
// deck sizes, and player decks for game initialization.
//
// Group: Ø51
//
// Author: Miloš Đukarić, Florian Kerman, Stefan Jović
// ------------------------------------------------------------------------
#include "ConfigParser.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

using namespace std; // for convenience inside this file

// ------------------------------------------------------------------------
//
// Constructor that automatically parses the given config file path.
//
// @param filePath Path to the game configuration file
//
// ------------------------------------------------------------------------
GameConfigParser::GameConfigParser(const string &filePath)
{
  // Parse the game configuration file upon construction
  parseFile(filePath);
}

// ------------------------------------------------------------------------
//
// Getter methods for game configuration constants.
//
// @return Corresponding int values as defined in the config file
//
// ------------------------------------------------------------------------
int GameConfigParser::getPlayerHealth() const { return playerHealth; }
int GameConfigParser::getMaxRounds() const { return maxRounds; }
int GameConfigParser::getDeckSize() const { return deckSize; }
int GameConfigParser::getManaPoolStart() const { return manaPoolStart; }

// ------------------------------------------------------------------------
//
// Returns the parsed deck configuration for Player 1 / Player 2.
//
// @return Reference to a vector of card IDs
//
// ------------------------------------------------------------------------
const vector<string> &GameConfigParser::getPlayer1Deck() const { return deck1; }
const vector<string> &GameConfigParser::getPlayer2Deck() const { return deck2; }

// ------------------------------------------------------------------------
//
// Parses the game configuration file step by step. Verifies magic header,
// loads constants, skips blank line, parses both deck lines, and validates
// that each deck has the required number of cards.
//
// @param filePath Path to the config file to parse
//
// @throws runtime_error on file I/O issues or format violations
//
// ------------------------------------------------------------------------
void GameConfigParser::parseFile(const string &filePath)
{
  ifstream in(filePath);
  if (!in.is_open())
  {
    throw runtime_error("Could not open game config file: " + filePath);
  }

  // 1) Read and verify the magic number
  string magic;
  getline(in, magic);
  if (magic != "GAME")
  {
    throw runtime_error("Invalid game config magic: " + magic);
  }

  // 2) Helper to read one integer per line
  auto readIntLine = [&](int &out)
  {
    string line;
    if (!getline(in, line)) throw runtime_error("Unexpected end of game config while reading integers");
    out = stoi(line);
  };

  // Read the four fixed constants
  readIntLine(playerHealth); // <PLAYER_HEALTH>
  readIntLine(maxRounds); // <MAX_ROUNDS>
  readIntLine(deckSize); // <DECK_SIZE>
  readIntLine(manaPoolStart); // <MANA_POOL_SIZE>
  manaPoolStart++;

  // 3) Skip the blank separator line
  string blank;
  getline(in, blank);

  // 4) Read each player's deck line (semicolon-delimited IDs)
  string line1, line2;
  if (!getline(in, line1) || !getline(in, line2))
  {
    throw runtime_error("Missing deck lines in game config");
  }
  deck1 = splitIds(line1);
  deck2 = splitIds(line2);

  // 5) Validate that each deck has the correct number of cards
  if ((int) deck1.size() != deckSize || (int) deck2.size() != deckSize)
  {
    throw runtime_error("Deck size mismatch in game config");
  }
}

// ------------------------------------------------------------------------
//
// Helper method to split a semicolon-separated line of card IDs.
//
// @param line Input line from config file (e.g. "ID1;ID2;ID3")
// @return Vector of card ID strings
//
// ------------------------------------------------------------------------
vector<string> GameConfigParser::splitIds(const string &line) const
{
  vector<string> ids;
  string token;
  stringstream ss(line);

  // Split on ';' and collect non-empty IDs
  while (getline(ss, token, ';'))
  {
    if (!token.empty()) ids.push_back(token);
  }
  return ids;
}
