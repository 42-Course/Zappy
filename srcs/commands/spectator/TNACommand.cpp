#include "commands/spectator/TNACommand.hpp"
#include "core/Team.hpp"
#include "core/World.hpp"
#include <sstream>

namespace Zappy {

TNACommand::TNACommand(World &world, const std::vector<std::string> &tokens,
                       ClientConnection *client)
    : Command("tna", CommandType::SPECTATOR, 0, client), world_(world) {
  if (!parseArgs(tokens)) {
    setStatus(CommandStatus::INVALID);
  }
}

bool TNACommand::parseArgs(const std::vector<std::string> &args) {
  if (!Command::parseArgs(args))
    return false;
  return validateArgCount(0); // No args expected
}

CommandStatus TNACommand::execute() {
  for (const auto &team : world_.getTeams()) {
    if (team) {
      std::string line = "tna " + team->getName() + "\n";
      getClient()->sendData(line);
    }
  }

  logCommand("Sent all team names");
  return CommandStatus::COMPLETED;
}

} // namespace Zappy
