#include "commands/player/RightCommand.hpp"
#include "core/Player.hpp"

namespace Zappy {

RightCommand::RightCommand(World &world, const std::vector<std::string> &tokens,
                           ClientConnection *client)
    : Command("right", CommandType::PLAYER, 7, client), world_(world) {
  if (!parseArgs(tokens)) {
    setStatus(CommandStatus::INVALID);
  }
}

bool RightCommand::parseArgs(const std::vector<std::string> &args) {
  return Command::parseArgs(args) && validateArgCount(0);
}

CommandStatus RightCommand::execute() {
  Player *player = getClient()->getPlayer();
  if (!player) {
    setErrorMessage("Player not set");
    return CommandStatus::FAILED;
  }

  Direction dir = player->getDirection();
  switch (dir) {
  case Direction::NORTH:
    player->setDirection(Direction::EAST);
    break;
  case Direction::EAST:
    player->setDirection(Direction::SOUTH);
    break;
  case Direction::SOUTH:
    player->setDirection(Direction::WEST);
    break;
  case Direction::WEST:
    player->setDirection(Direction::NORTH);
    break;
  }

  getClient()->sendData("ok\n");
  logCommand("Player turned right");
  return CommandStatus::COMPLETED;
}

} // namespace Zappy
