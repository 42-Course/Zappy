#include "commands/player/DropCommand.hpp"
#include "core/Player.hpp"
#include "core/Tile.hpp"
#include "core/utils.hpp"

namespace Zappy {

DropCommand::DropCommand(World& world, const std::vector<std::string>& tokens, ClientConnection* client)
  : Command("drop", CommandType::PLAYER, 7, client), world_(world) {
  if (!parseArgs(tokens)) {
    setStatus(CommandStatus::INVALID);
  }
}

bool DropCommand::parseArgs(const std::vector<std::string>& args) {
  if (!Command::parseArgs(args) || !validateArgCount(1))
    return false;

  std::string type_name = getArg(0);
  transform(type_name.begin(), type_name.end(), type_name.begin(),
              ::tolower);
  try {
    type_ = Resource::stringToType(type_name);
  } catch (std::exception &e) {
    setErrorMessage(e.what());
    return false;
  }
  return true;
}

CommandStatus DropCommand::execute() {
  Player* player = getClient()->getPlayer();
  if (!player) {
      setErrorMessage("Player not set");
      return CommandStatus::FAILED;
  }

  int px = player->getX();
  int py = player->getY();

  Map& map = world_.getMap();

  bool success = player->removeResource(type_);

  if (!success) {
    getClient()->sendData("ko\n");  
    setErrorMessage("Unable to take resource");
    return CommandStatus::FAILED;
  }
  map.addResource(px, py, type_);
  getClient()->sendData("ok\n");  
  logCommand("Player takes resource");
  return CommandStatus::COMPLETED;
}


}
