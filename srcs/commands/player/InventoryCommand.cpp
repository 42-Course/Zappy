#include "commands/player/InventoryCommand.hpp"
#include "core/Player.hpp"
#include "core/World.hpp"

namespace Zappy {

InventoryCommand::InventoryCommand(World &world, const std::vector<std::string> &tokens,
                                   ClientConnection *client)
    : Command("inventory", CommandType::PLAYER, 1, client), world_(world) {
  if (!parseArgs(tokens)) {
    setStatus(CommandStatus::INVALID);
  }
}

bool InventoryCommand::parseArgs(const std::vector<std::string> &args) {
  if (!Command::parseArgs(args))
    return false;
  return validateArgCount(0); // inventory takes no arguments
}

CommandStatus InventoryCommand::execute() {
  Player *player = getClient()->getPlayer();
  if (!player) {
    setErrorMessage("Player not found");
    return CommandStatus::FAILED;
  }

  std::string inventoryStr = player->getInventory().toInventoryString();
  getClient()->sendData(inventoryStr);
  logCommand("Sent inventory");
  return CommandStatus::COMPLETED;
}

} // namespace Zappy
