#pragma once

#include "commands/Command.hpp"

namespace Zappy {

class World;

class TNACommand : public Command {
public:
    TNACommand(World& world, const std::vector<std::string>& tokens, ClientConnection* client);

    bool parseArgs(const std::vector<std::string>& args) override;
    CommandStatus execute() override;

private:
    World& world_;
};

}
