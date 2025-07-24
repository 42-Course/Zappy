#include "commands/CommandHandler.hpp"

namespace Zappy {

void CommandHandler::queueCommand(CommandPtr command) {
  if (!command)
    return;

  std::lock_guard<std::mutex> lock(queueMutex_);
  commandQueue_.push(command);
  Logger::getInstance().debug("Queued command: " + command->getName());
}

void CommandHandler::processCommands() {
  std::lock_guard<std::mutex> lock(queueMutex_);

  while (!commandQueue_.empty()) {
    auto command = commandQueue_.front();

    if (!isCommandReady(command)) {
      break;
    }

    commandQueue_.pop();

    command->setStatus(CommandStatus::EXECUTING);
    Logger::getInstance().debug("Executing command: " + command->getName());

    CommandStatus status = command->execute();
    command->setStatus(status);

    if (status == CommandStatus::FAILED || status == CommandStatus::INVALID) {
      Logger::getInstance().error("Command " + command->getName() +
                                  " failed: " + command->getErrorMessage());
    }
  }
}

bool CommandHandler::hasCommands() const {
  std::lock_guard<std::mutex> lock(queueMutex_);
  return !commandQueue_.empty();
}

size_t CommandHandler::queueSize() const {
  std::lock_guard<std::mutex> lock(queueMutex_);
  return commandQueue_.size();
}

bool CommandHandler::isCommandReady(const CommandPtr &command) {
  // For now, all commands are ready immediately
  // In the future, implement time cost checking here
  (void)command;
  return true;
}

} // namespace Zappy