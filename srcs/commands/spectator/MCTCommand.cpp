#include "commands/spectator/MCTCommand.hpp"
#include "core/Map.hpp"
#include "core/Tile.hpp"
#include "core/World.hpp"
#include <sstream>

namespace Zappy {

MCTCommand::MCTCommand(World &world, const std::vector<std::string> &tokens,
                       ClientConnection *client)
    : Command("mct", CommandType::SPECTATOR, 0, client), world_(world) {
  if (!parseArgs(tokens)) {
    setStatus(CommandStatus::INVALID);
  }
}

bool MCTCommand::parseArgs(const std::vector<std::string> &args) {
  if (!Command::parseArgs(args))
    return false;
  return validateArgCount(0);
}

CommandStatus MCTCommand::execute() {
  const Map &map = world_.getMap();

  for (int y = 0; y < map.getHeight(); ++y) {
    for (int x = 0; x < map.getWidth(); ++x) {
      const Tile *tile = map.getTile(x, y);
      if (tile) {
        getClient()->sendData(tile->toBctString(x, y));
      }
    }
  }

  logCommand("Sent map content");
  return CommandStatus::COMPLETED;
}

} // namespace Zappy
