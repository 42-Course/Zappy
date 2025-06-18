#include "commands/spectator/SSTCommand.hpp"
#include "services/Logger.hpp"
#include <iostream>
#include <sstream>

namespace Zappy {

SSTCommand::SSTCommand(GameLoop& gameLoop, const std::vector<std::string>& tokens, ClientConnection* client)
    : Command("sst", CommandType::SPECTATOR, 0, client), gameLoop_(gameLoop), tickRate_(0) {
    if (!parseArgs(tokens)) {
        setStatus(CommandStatus::INVALID);
    }
}

bool SSTCommand::parseArgs(const std::vector<std::string>& args) {
    if (!Command::parseArgs(args))
        return false;

    if (!validateArgCount(1))
        return false;

    const auto& parsedArgs = getArgs();
    try {
        tickRate_ = std::stoi(parsedArgs[0]);
        if (tickRate_ > 300 || tickRate_ <= 0) {
          setErrorMessage("sst must be between 1 and 300");
          return false;
        }
    } catch (...) {
        return false;
    }

    return true;
}

CommandStatus SSTCommand::execute() {
    gameLoop_.setTickRate(tickRate_);

    getClient()->sendData(gameLoop_.toSgtString());

    logCommand("Set tick rate to " + std::to_string(tickRate_));
    return CommandStatus::COMPLETED;
}

} // namespace Zappy
