#include "commands/spectator/PINCommand.hpp"
#include "core/Player.hpp"
#include "core/World.hpp"

namespace Zappy {

PINCommand::PINCommand(World &world, const std::vector<std::string> &tokens,
                       ClientConnection *client)
    : Command("pin", CommandType::SPECTATOR, 0, client), world_(world), playerId_(0) {
  if (!parseArgs(tokens)) {
    setStatus(CommandStatus::INVALID);
  }
}

bool PINCommand::parseArgs(const std::vector<std::string> &args) {
  if (!Command::parseArgs(args))
    return false;

  if (!validateArgCount(1))
    return false;

  try {
    playerId_ = std::stoi(getArg(0));
  } catch (...) {
    setErrorMessage("Invalid player ID");
    return false;
  }

  return true;
}

CommandStatus PINCommand::execute() {
  const Player *player = world_.getPlayer(playerId_);
  if (!player) {
    setErrorMessage("Player not found");
    return CommandStatus::FAILED;
  }

  getClient()->sendData(player->toPinString());
  logCommand("Sent inventory for player " + std::to_string(playerId_));
  return CommandStatus::COMPLETED;
}

} // namespace Zappy
