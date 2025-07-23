#include "commands/player/KickCommand.hpp"
#include "core/Player.hpp"

namespace Zappy {

KickCommand::KickCommand(World& world, const std::vector<std::string>& tokens, ClientConnection* client)
    : Command("kick", CommandType::PLAYER, 7, client), world_(world) {
    if (!parseArgs(tokens)) {
        setStatus(CommandStatus::INVALID);
    }
}

bool KickCommand::parseArgs(const std::vector<std::string>& args) {
    return Command::parseArgs(args) && validateArgCount(0);
}

CommandStatus KickCommand::execute() {
    Player* player = getClient()->getPlayer();
    if (!player) {
        setErrorMessage("Player not set");
        return CommandStatus::FAILED;
    }

    for (const auto & [id, player] : world_.getPlayers()) {
      if (player.get()->getPosition() == player->getPosition()) {
        Player *player = world_.getPlayer(id);
        if (player)
          player->kick(player->getDirection());
      }
    }

    logCommand("Player kicked everyone in his tile");
    return CommandStatus::COMPLETED;
}

}
