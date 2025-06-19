// --------------------------- MessageConfigParser.hpp ---------------------------
//
// Parses a MESSAGE config file and provides lookup functionality for game messages.
// Each message is defined by a key (I_, E_, or D_) and formatted accordingly.
//
// Group: 051
//
// Author: Miloš Đukarić, Florian Kerman, Stefan Jović
// ------------------------------------------------------------------------
#pragma once

#include <string>
#include <unordered_map>

using namespace std; // bring std symbols into this header for brevity

//---------------------------------------------------------------------------------------------------------------------
///
/// Class for parsing and retrieving formatted messages from a configuration file.
/// Supports [INFO], [ERROR], and plain descriptive message types.
///
/// The config file must:
///  - Start with "MESSAGE" magic line
///  - Contain lines in format KEY:TEXT
///
/// Key types:
///  - I_<KEY> → Info message → printed with "[INFO] "
///  - E_<KEY> → Error message → printed with "[ERROR] "
///  - D_<KEY> → Descriptive → no prefix
///
//---------------------------------------------------------------------------------------------------------------------
class MessageConfigParser
{
public:
  //---------------------------------------------------------------------------------------------------------------------
  ///
  /// Constructor: parses the message config file.
  ///
  /// @param filePath Path to a MESSAGE config file
  ///
  /// @throws runtime_error if the file is missing or improperly formatted
  ///
  //---------------------------------------------------------------------------------------------------------------------
  MessageConfigParser(const string &filePath);

  //---------------------------------------------------------------------------------------------------------------------
  ///
  /// Checks whether a given message key exists.
  ///
  /// @param key The message key (e.g., "I_WELCOME")
  ///
  /// @return true if key is present, false otherwise
  ///
  //---------------------------------------------------------------------------------------------------------------------
  bool contains(const string &key) const;

  //---------------------------------------------------------------------------------------------------------------------
  ///
  /// Retrieves the raw message text without any prefix or newline.
  ///
  /// @param key The message key
  ///
  /// @return Raw message text as stored in the config
  ///
  /// @throws std::out_of_range if the key is not found
  ///
  //---------------------------------------------------------------------------------------------------------------------
  string getRaw(const string &key) const;

  //---------------------------------------------------------------------------------------------------------------------
  ///
  /// Retrieves the full formatted message (prefix + text + newline).
  ///
  /// @param key The message key
  ///
  /// @return Formatted message with correct prefix and trailing newline
  ///
  /// @throws std::out_of_range if the key is not found
  ///
  //---------------------------------------------------------------------------------------------------------------------
  string getMessage(const string &key) const;

private:
  // Holds KEY → TEXT mappings
  unordered_map<string, string> messages;

  //---------------------------------------------------------------------------------------------------------------------
  ///
  /// Loads and parses the config file into internal key-message mapping.
  ///
  /// @param filePath Path to the config file
  ///
  /// @throws runtime_error if the file is not found or invalid
  ///
  //---------------------------------------------------------------------------------------------------------------------
  void parseFile(const string &filePath);

  //---------------------------------------------------------------------------------------------------------------------
  ///
  /// Determines the message prefix based on key format.
  ///
  /// @param key Message key to evaluate
  ///
  /// @return Prefix string: "[INFO] ", "[ERROR] ", or "" if descriptive
  ///
  //---------------------------------------------------------------------------------------------------------------------
  string getPrefix(const string &key) const;
};
