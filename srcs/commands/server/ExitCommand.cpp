#include "commands/server/ExitCommand.hpp"
#include "core/Engine.hpp"
#include <iostream>

namespace Zappy {

ExitCommand::ExitCommand(Engine &engine, const std::vector<std::string> &tokens,
                         ClientConnection *client)
    : Command("exit", client), engine_(engine) {
  // Skip validation if we have no tokens (direct command creation)
  if (!tokens.empty()) {
    // Skip the first token (command name) when parsing args
    std::vector<std::string> args(tokens.begin() + 1, tokens.end());
    if (!parseArgs(args)) {
      setErrorMessage("Exit command takes no arguments");
      setStatus(CommandStatus::INVALID);
    }
  }
}

CommandStatus ExitCommand::execute() {
  if (getStatus() == CommandStatus::INVALID) {
    std::cout << "Error: " << getErrorMessage() << "\n";
    std::cout << "Usage: " << getUsage() << "\n";
    return CommandStatus::FAILED;
  }

  std::cout << "Stopping server...\n";
  engine_.stop();
  return CommandStatus::COMPLETED;
}

bool ExitCommand::parseArgs(const std::vector<std::string> &args) {
  // Exit takes no arguments
  return args.empty();
}

std::string ExitCommand::getDescription() const {
  return "Stop the server and exit";
}

std::string ExitCommand::getUsage() const {
  return "exit - Gracefully shutdown the server";
}

} // namespace Zappy