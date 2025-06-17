#pragma once

#include "commands/Command.hpp"

namespace Zappy {

class World;

class PINCommand : public Command {
public:
    PINCommand(World& world, const std::vector<std::string>& tokens, ClientConnection* client);

    bool parseArgs(const std::vector<std::string>& args) override;
    CommandStatus execute() override;

private:
    World& world_;
    int playerId_;
};

}
