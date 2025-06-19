#include "Game.hpp"
#include <iostream>
#include <fstream>
#include <new>         // For std::bad_alloc
#include <exception>   // For std::exception

using namespace std;

/**
 * @brief Entry point for the card game application.
 *
 * This main function processes the command-line arguments, validates configuration files,
 * creates the Game object, and runs the game loop. It handles errors gracefully and
 * provides meaningful exit codes for various failure scenarios.
 *
 * @param argc Number of command-line arguments (should be 3: program name + 2 configs)
 * @param argv Array of command-line argument strings
 * @return int Exit code: 0 = success, 1 = memory error, 2 = invalid usage,
 *                  3 = config file error, 5 = unknown fatal error
 */
int main(int argc, char **argv)
{
  try
  {
    // ---------------------------------------------------------------------
    // Check the number of command-line arguments.
    // Expect exactly 2 arguments: the game config file and the message config file.
    // If not provided, print error message and exit with code 2.
    // ---------------------------------------------------------------------
    if (argc != 3)
    {
      cout << "[ERROR] Wrong number of parameters.\n";
      return 2;
    }

    // ---------------------------------------------------------------------
    // Extract the paths to the configuration files from the arguments.
    // gameCfg: Path to the game configuration (cards, rules, etc.)
    // msgCfg:  Path to the message configuration (custom messages)
    // ---------------------------------------------------------------------
    string gameCfg = argv[1];
    string msgCfg = argv[2];

    // ---------------------------------------------------------------------
    // Check if the game configuration file exists and can be opened.
    // If the file cannot be opened, print an error and exit with code 3.
    // ---------------------------------------------------------------------
    ifstream gameFile(gameCfg);
    if (!gameFile.is_open())
    {
      cout << "[ERROR] Invalid file (" << gameCfg << ")." << endl;
      return 3;
    }

    // ---------------------------------------------------------------------
    // Check if the message configuration file exists and can be opened.
    // If the file cannot be opened, print an error and exit with code 3.
    // ---------------------------------------------------------------------
    ifstream msgFile(msgCfg);
    if (!msgFile.is_open())
    {
      cerr << "[ERROR] Message config file '" << msgCfg << "' could not be opened." << endl;
      return 3;
    }

    // ---------------------------------------------------------------------
    // Initialize and start the main game loop.
    // Game object is constructed with the provided config file paths.
    // The return value of game.run() determines the program's exit code.
    // ---------------------------------------------------------------------
    Game game(gameCfg, msgCfg);
    return game.run();
  }
  // -------------------------------------------------------------------------
  // Handle out-of-memory errors specifically (bad_alloc).
  // -------------------------------------------------------------------------
  catch (const std::bad_alloc &e)
  {
    cerr << "[ERROR] Not enough memory: " << e.what() << endl;
    return 1;
  }
  // -------------------------------------------------------------------------
  // Handle other standard exceptions (e.g., file errors, logic errors).
  // -------------------------------------------------------------------------
  catch (const std::exception &e)
  {
    cerr << "[ERROR] Not enough memory: " << e.what() << endl;
    return 1;
  }
  // -------------------------------------------------------------------------
  // Catch-all handler for any other unknown fatal errors.
  // -------------------------------------------------------------------------
  catch (...)
  {
    cerr << "[ERROR] Unknown fatal error occurred." << endl;
    return 5;
  }
}
