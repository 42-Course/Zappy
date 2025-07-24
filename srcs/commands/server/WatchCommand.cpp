#include "commands/server/WatchCommand.hpp"
#include "core/Engine.hpp"
#include <iostream>

namespace Zappy {

WatchCommand::WatchCommand(Engine &engine, const std::vector<std::string> &tokens,
                           WatchService &watchService, ClientConnection *client)
    : Command("watch", client), engine_(engine), watchService_(watchService) {
  // Skip validation if we have no tokens (direct command creation)
  if (!tokens.empty()) {
    // Skip the first token (command name) when parsing args
    std::vector<std::string> args(tokens.begin() + 1, tokens.end());
    if (!parseArgs(args)) {
      setErrorMessage("Watch command takes at most one argument");
      setStatus(CommandStatus::INVALID);
    }
  }
}

CommandStatus WatchCommand::execute() {
  if (getStatus() == CommandStatus::INVALID) {
    std::cout << "Error: " << getErrorMessage() << "\n";
    std::cout << "Usage: " << getUsage() << "\n";
    return CommandStatus::FAILED;
  }

  bool enable;

  // No argument means toggle
  if (args_.empty()) {
    enable = !watchService_.isEnabled();
  } else {
    const std::string &arg = args_[0];
    if (arg == "on") {
      enable = true;
    } else if (arg == "off") {
      enable = false;
    } else {
      std::cout << "Invalid argument. Use 'on' or 'off'\n";
      return CommandStatus::FAILED;
    }
  }

  if (enable) {
    watchService_.enable();
    std::cout << "Watch mode enabled\n";
  } else {
    watchService_.disable();
    std::cout << "Watch mode disabled\n";
  }

  return CommandStatus::COMPLETED;
}

bool WatchCommand::parseArgs(const std::vector<std::string> &args) {
  // Clear previous arguments
  args_.clear();

  // Watch can take 0 or 1 argument
  if (args.size() > 1) {
    return false;
  }

  // Validate argument if present
  if (!args.empty()) {
    const auto &arg = args[0];
    if (arg != "on" && arg != "off") {
      setErrorMessage("Watch argument must be 'on' or 'off'");
      return false;
    }
  }

  // Store arguments if any
  args_ = args;
  return true;
}

} // namespace Zappy