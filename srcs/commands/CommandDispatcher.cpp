#include "commands/CommandDispatcher.hpp"
#include "net/ClientConnection.hpp"
#include <algorithm>
#include <sstream>

namespace Zappy {

void CommandDispatcher::registerHandler(const std::string &name, CommandHandler handler) {
  handlers_[name] = std::move(handler);
}

std::unique_ptr<ICommand> CommandDispatcher::dispatch(const std::string &line,
                                                      ClientConnection *client) const {
  std::string commandName = extractCommandName(line);
  auto it                 = handlers_.find(commandName);
  if (it == handlers_.end()) {
    return nullptr;
  }

  std::vector<std::string> tokens;
  std::istringstream iss(line);
  std::string token;
  while (iss >> token) {
    tokens.push_back(token);
  }

  return it->second(tokens, client);
}

std::string CommandDispatcher::extractCommandName(const std::string &line) {
  std::istringstream iss(line);
  std::string command;
  iss >> command;
  return command;
}

std::vector<std::string> CommandDispatcher::getCommandNames() const {
  std::vector<std::string> names;
  for (const auto &pair : handlers_) {
    names.push_back(pair.first);
  }
  std::sort(names.begin(), names.end());
  return names;
}

std::unique_ptr<ICommand> CommandDispatcher::createCommand(const std::string &name,
                                                           const std::vector<std::string> &tokens,
                                                           ClientConnection *client) const {
  auto it = handlers_.find(name);
  if (it == handlers_.end()) {
    return nullptr;
  }
  return it->second(tokens, client);
}
} // namespace Zappy
