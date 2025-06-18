#include "commands/player/LeftCommand.hpp"
#include "core/World.hpp"
#include "core/Player.hpp"

namespace Zappy {

LeftCommand::LeftCommand(World& world, const std::vector<std::string>& tokens, ClientConnection* client)
    : Command("left", CommandType::PLAYER, 7, client), world_(world) {
    if (!parseArgs(tokens)) {
        setStatus(CommandStatus::INVALID);
    }
}

bool LeftCommand::parseArgs(const std::vector<std::string>& args) {
    return Command::parseArgs(args) && validateArgCount(0);
}

CommandStatus LeftCommand::execute() {
    Player* player = getClient()->getPlayer();
    if (!player) {
        setErrorMessage("Player not found");
        return CommandStatus::FAILED;
    }

    Direction dir = player->getDirection();
    switch (dir) {
        case Direction::NORTH: player->setDirection(Direction::WEST); break;
        case Direction::WEST: player->setDirection(Direction::SOUTH); break;
        case Direction::SOUTH: player->setDirection(Direction::EAST); break;
        case Direction::EAST: player->setDirection(Direction::NORTH); break;
    }

    getClient()->sendData("ok\n");
    logCommand("Player turned left");
    return CommandStatus::COMPLETED;
}

}
