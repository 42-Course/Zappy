#include "commands/server/HelpCommand.hpp"
#include "core/Engine.hpp"
#include <iomanip>
#include <iostream>

namespace Zappy {

HelpCommand::HelpCommand(Engine &engine, const std::vector<std::string> &tokens,
                         ClientConnection *client)
    : Command("help", client), engine_(engine) {
  // Skip validation if we have no tokens (direct command creation)
  if (!tokens.empty()) {
    if (!parseArgs(tokens)) {
      setErrorMessage("Help command takes at most one argument");
      setStatus(CommandStatus::INVALID);
    }
  }
}

CommandStatus HelpCommand::execute() {
  if (getStatus() == CommandStatus::INVALID) {
    std::cout << "Error: " << getErrorMessage() << "\n";
    std::cout << "Usage: " << getUsage() << "\n";
    return CommandStatus::FAILED;
  }

  // Show warning if we ignored some arguments
  if (!getErrorMessage().empty()) {
    std::cout << "Warning: " << getErrorMessage() << "\n";
  }

  // If specific command help was requested
  if (!args_.empty()) {
    const std::string &targetCommand = args_[0];
    // Try to create the command to get its help information
    auto command = engine_.getNetwork().createServerCommand(targetCommand, {}, nullptr);
    if (command) {
      std::cout << targetCommand << " - " << command->getDescription() << "\n";
      std::cout << "Usage: " << command->getUsage() << "\n";
    } else {
      std::cout << "Command '" << targetCommand << "' not found.\n";
      return CommandStatus::FAILED;
    }
    return CommandStatus::COMPLETED;
  }

  // Display help for all commands
  std::cout << "Available commands:\n";
  const auto &commandNames = engine_.getNetwork().getServerCommandNames();
  for (const auto &cmdName : commandNames) {
    auto command = engine_.getNetwork().createServerCommand(cmdName, {}, nullptr);
    if (command) {
      std::cout << std::setw(15) << std::left << cmdName << " - " << command->getDescription()
                << "\n";
    }
  }

  std::cout << "\nType 'help <command>' for detailed usage information.\n";
  return CommandStatus::COMPLETED;
}

bool HelpCommand::parseArgs(const std::vector<std::string> &args) {
  // Clear previous arguments
  args_.clear();

  // Help can take 0 or 1 argument, if more arguments are provided,
  // just use the first one and warn the user
  if (args.size() > 1) {
    args_.push_back(args[0]);
    setErrorMessage("Help command takes only one argument, ignoring additional arguments");
    return true; // Still valid, but with warning
  }

  // Store arguments if any
  args_ = args;
  return true;
}

std::string HelpCommand::getDescription() const {
  return "Display available commands and their usage";
}

std::string HelpCommand::getUsage() const {
  return "help [command] - Show help for all commands or specific command";
}

} // namespace Zappy