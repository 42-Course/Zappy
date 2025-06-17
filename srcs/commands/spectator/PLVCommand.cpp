#include "commands/spectator/PLVCommand.hpp"
#include "core/World.hpp"
#include "core/Player.hpp"

#include <sstream>

namespace Zappy {

PLVCommand::PLVCommand(World& world, const std::vector<std::string>& tokens, ClientConnection* client)
    : Command("plv", CommandType::SPECTATOR, 0, client), world_(world), playerId_(0) {
    if (!parseArgs(tokens)) {
        setStatus(CommandStatus::INVALID);
    }
}

bool PLVCommand::parseArgs(const std::vector<std::string>& args) {
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

CommandStatus PLVCommand::execute() {
    const Player* player = world_.getPlayer(playerId_);
    if (!player) {
        setErrorMessage("Player not found");
        return CommandStatus::FAILED;
    }

    getClient()->sendData(player->toPlvString());
    logCommand("Sent player level for player " + std::to_string(playerId_));
    return CommandStatus::COMPLETED;
}

}
