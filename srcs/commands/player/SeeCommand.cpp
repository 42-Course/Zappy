#include "commands/player/SeeCommand.hpp"
#include "core/Player.hpp"
#include "core/Tile.hpp"
#include "core/utils.hpp"

namespace Zappy {

SeeCommand::SeeCommand(World& world, const std::vector<std::string>& tokens, ClientConnection* client)
    : Command("see", CommandType::PLAYER, 7, client), world_(world) {
    if (!parseArgs(tokens)) {
        setStatus(CommandStatus::INVALID);
    }
}

bool SeeCommand::parseArgs(const std::vector<std::string>& args) {
    return Command::parseArgs(args) && validateArgCount(0);
}

void computeTileInView(int px, int py, Direction dir, int level, int offset, int& outX, int& outY) {
    switch (dir) {
        case Direction::NORTH:
            outX = px + offset;
            outY = py - level;
            break;
        case Direction::SOUTH:
            outX = px - offset;
            outY = py + level;
            break;
        case Direction::EAST:
            outX = px + level;
            outY = py + offset;
            break;
        case Direction::WEST:
            outX = px - level;
            outY = py - offset;
            break;
    }
}


CommandStatus SeeCommand::execute() {
    Player* player = getClient()->getPlayer();
    if (!player) {
        setErrorMessage("Player not set");
        return CommandStatus::FAILED;
    }

    int px = player->getX();
    int py = player->getY();
    int level = player->getLevel();
    Direction dir = player->getDirection();
    const Map& map = world_.getMap();

    std::vector<std::string> result;

    for (int l = 0; l <= level; ++l) {
        for (int offset = -l; offset <= l; ++offset) {
            int tx, ty;
            std::vector<std::string> contents;
            computeTileInView(px, py, dir, l, offset, tx, ty);
            map.normalizeCoordinates(tx, ty);  // safe even if not infinite
            const Tile* tile = map.getTile(tx, ty);
            contents.push_back(tile ? tile->resourcesToString() : "");

            for (auto&[id, other_player] : world_.getPlayers()) {
                if (other_player->getId() == player->getId())
                    continue;
                auto [x, y] = other_player->getPosition();
                if (x == tx && y == ty)
                    contents.push_back("player");
            }
            result.push_back(join(contents, " "));
        }
    }

    std::string output = "{" + join(result, ", ") + "}\n";
    getClient()->sendData(output);
    logCommand("Player sees: " + output);
    return CommandStatus::COMPLETED;
}


}
