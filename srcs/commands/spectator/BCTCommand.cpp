#include "commands/spectator/BCTCommand.hpp"
#include "core/Map.hpp"
#include "core/Tile.hpp"
#include "core/World.hpp"
#include <iostream>
#include <sstream>

namespace Zappy {

BCTCommand::BCTCommand(World &world, const std::vector<std::string> &tokens,
                       ClientConnection *client)
    : Command("bct", CommandType::SPECTATOR, 2, client), world_(world), x_(0), y_(0) {
  if (!parseArgs(tokens)) {
    // setErrorMessage("bct requires exactly two arguments");
    setStatus(CommandStatus::INVALID);
    std::cout << "[" << getErrorMessage() << "]" << std::endl;
  }
}

bool BCTCommand::parseArgs(const std::vector<std::string> &args) {
  if (!Command::parseArgs(args))
    return false;

  if (!validateArgCount(2))
    return false;

  const auto &parsedArgs = getArgs();

  try {
    x_ = std::stoi(parsedArgs[0]);
    y_ = std::stoi(parsedArgs[1]);
  } catch (...) {
    return false;
  }

  return true;
}

CommandStatus BCTCommand::execute() {
  const Map &map   = world_.getMap();
  const Tile *tile = map.getTile(x_, y_);

  if (!tile) {
    setErrorMessage("Invalid coordinates");
    return CommandStatus::FAILED;
  }

  std::string result = tile->toBctString(x_, y_);
  getClient()->sendData(result);
  logCommand("Sent tile content at (" + std::to_string(x_) + ", " + std::to_string(y_) + ")");
  return CommandStatus::COMPLETED;
}

} // namespace Zappy
