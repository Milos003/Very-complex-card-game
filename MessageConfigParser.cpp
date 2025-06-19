// --------------------------- MessageConfigParser.cpp ---------------------------
//
// Parses and manages key-value message configuration used in the game.
// Messages include [INFO] and [ERROR] prefixes and are read from a config file.
//
// Group: 051
//
// Author: Miloš Đukarić, Florian Kerman, Stefan Jović
// ------------------------------------------------------------------------
#include "MessageConfigParser.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

using namespace std; // bring std symbols into this file for clarity

//---------------------------------------------------------------------------------------------------------------------
///
/// Constructor: parses the given message configuration file.
///
/// @param filePath Path to the message configuration file
///
//---------------------------------------------------------------------------------------------------------------------
MessageConfigParser::MessageConfigParser(const string &filePath)
{
  parseFile(filePath);
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Parses the message config file, validating the magic and reading key:value entries.
///
/// @param filePath Path to the config file to be parsed
///
//---------------------------------------------------------------------------------------------------------------------
void MessageConfigParser::parseFile(const string &filePath)
{
  ifstream in(filePath);
  if (!in.is_open())
  {
    throw runtime_error("Could not open message config file: " + filePath);
  }

  // 1) Read and trim magic number line (handles \r and spaces)
  string magic;
  getline(in, magic);
  while (!magic.empty() && (magic.back() == '\r' || magic.back() == ' '))
  {
    magic.pop_back();
  }
  if (magic != "MESSAGE")
  {
    throw runtime_error("Invalid message config magic: " + magic);
  }

  // 2) Parse each subsequent non-empty line as KEY:TEXT
  string line;
  while (getline(in, line))
  {
    if (line.empty()) continue;
    auto pos = line.find(':');
    if (pos == string::npos) continue; // skip malformed lines

    // Extract key and text
    string key = line.substr(0, pos);
    string text = line.substr(pos + 1);

    // Trim any stray carriage return from both key and text
    if (!key.empty() && key.back() == '\r') key.pop_back();
    if (!text.empty() && text.back() == '\r') text.pop_back();

    messages[key] = text;
  }
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Checks whether a message key is present in the configuration.
///
/// @param key Message key to check
///
/// @return true if the key exists, false otherwise
///
//---------------------------------------------------------------------------------------------------------------------
bool MessageConfigParser::contains(const string &key) const
{
  return messages.find(key) != messages.end();
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Retrieves the raw message string (no prefix or newline).
///
/// @param key Message key to look up
///
/// @return Raw message text
///
/// @throws std::out_of_range if key is not found
///
//---------------------------------------------------------------------------------------------------------------------
string MessageConfigParser::getRaw(const string &key) const
{
  return messages.at(key);
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Determines the message prefix based on key naming convention (I_, E_, or none).
///
/// @param key Message key
///
/// @return Prefix string such as "[INFO] ", "[ERROR] ", or empty string
///
//---------------------------------------------------------------------------------------------------------------------
string MessageConfigParser::getPrefix(const string &key) const
{
  if (key.rfind("I_", 0) == 0) return "[INFO] ";
  if (key.rfind("E_", 0) == 0) return "[ERROR] ";
  return string();
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Returns the complete formatted message with prefix and newline.
///
/// @param key Message key
///
/// @return Full formatted message with appropriate prefix and newline
///
//---------------------------------------------------------------------------------------------------------------------
string MessageConfigParser::getMessage(const string &key) const
{
  const auto &raw = getRaw(key);
  return getPrefix(key) + raw + "\n";
}
