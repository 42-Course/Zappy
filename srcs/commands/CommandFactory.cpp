#include "commands/CommandFactory.hpp"
#include "net/ClientConnection.hpp"
#include "services/Logger.hpp"

namespace Zappy {

CommandFactory &CommandFactory::getInstance() {
  static CommandFactory instance;
  return instance;
}

void CommandFactory::registerCommand(const std::string &name, CommandType type,
                                     CommandCreator creator) {
  auto key = std::make_pair(name, type);
  if (commands_.find(key) != commands_.end()) {
    Logger::getInstance().warning("Command " + name + " already registered, overwriting");
  }
  commands_[key] = creator;
  Logger::getInstance().debug("Registered command: " + name);
}

CommandPtr CommandFactory::createCommand(const std::string &name, CommandType type,
                                         ClientConnection *client,
                                         const std::vector<std::string> &args) {
  auto key   = std::make_pair(name, type);
  auto cmdIt = commands_.find(key);
  if (cmdIt == commands_.end()) {
    Logger::getInstance().error("Unknown command: " + name);
    return nullptr;
  }

  try {
    CommandPtr cmd = cmdIt->second(client, args);
    if (cmd && !cmd->parseArgs(args)) {
      cmd->setStatus(CommandStatus::INVALID);
    }
    return cmd;
  } catch (const std::exception &e) {
    Logger::getInstance().error("Failed to create command " + name + ": " + e.what());
    return nullptr;
  }
}

std::vector<CommandPtr> CommandFactory::getRegisteredCommands(CommandType type) {
  std::vector<CommandPtr> result;

  // Iterate through all commands and filter by type
  for (const auto &[key, creator] : commands_) {
    if (key.second == type) { // key.second is the CommandType
      if (auto cmd = creator(nullptr, {})) {
        result.push_back(cmd);
      }
    }
  }

  return result;
}

} // namespace Zappy