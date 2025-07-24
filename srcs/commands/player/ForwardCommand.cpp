#include "commands/player/ForwardCommand.hpp"
#include "core/Map.hpp"
#include "core/Player.hpp"
#include "core/Team.hpp"
#include "core/World.hpp"
#include <sstream>

namespace Zappy {

ForwardCommand::ForwardCommand(World &world, const std::vector<std::string> &tokens,
                               ClientConnection *client)
    : Command("forward", CommandType::PLAYER, 7, client), world_(world) {
  if (!parseArgs(tokens)) {
    setStatus(CommandStatus::INVALID);
  }
}

bool ForwardCommand::parseArgs(const std::vector<std::string> &args) {
  // Forward takes no arguments
  return Command::parseArgs(args) && validateArgCount(0);
}

CommandStatus ForwardCommand::execute() {
  Player *player = getClient()->getPlayer();
  if (!player) {
    setErrorMessage("No player bound to client");
    return CommandStatus::FAILED;
  }

  int x         = player->getX();
  int y         = player->getY();
  Direction dir = player->getDirection();

  switch (dir) {
  case Direction::NORTH:
    y -= 1;
    break;
  case Direction::EAST:
    x += 1;
    break;
  case Direction::SOUTH:
    y += 1;
    break;
  case Direction::WEST:
    x -= 1;
    break;
  }

  world_.getMap().normalizeCoordinates(x, y);
  player->setPosition(x, y);

  getClient()->sendData("ok\n");
  logCommand("Moved player forward to (" + std::to_string(x) + ", " + std::to_string(y) + ")");

  return CommandStatus::COMPLETED;
}

} // namespace Zappy
