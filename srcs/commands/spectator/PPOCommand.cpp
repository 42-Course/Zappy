#include "commands/spectator/PPOCommand.hpp"
#include "core/World.hpp"
#include "core/Player.hpp"

#include <sstream>

namespace Zappy {

PPOCommand::PPOCommand(World& world, const std::vector<std::string>& tokens, ClientConnection* client)
    : Command("ppo", CommandType::SPECTATOR, 0, client), world_(world), playerId_(0) {
    if (!parseArgs(tokens)) {
        setStatus(CommandStatus::INVALID);
    }
}

bool PPOCommand::parseArgs(const std::vector<std::string>& args) {
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

CommandStatus PPOCommand::execute() {
    const Player* player = world_.getPlayer(playerId_);
    if (!player) {
        setErrorMessage("Player not found");
        return CommandStatus::FAILED;
    }

    getClient()->sendData(player->toPpoString());
    logCommand("Sent player position for player " + std::to_string(playerId_));
    return CommandStatus::COMPLETED;
}

}
