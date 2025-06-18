#pragma once

#include "commands/Command.hpp"

namespace Zappy {

class World;

class LeftCommand : public Command {
public:
    LeftCommand(World& world, const std::vector<std::string>& tokens, ClientConnection* client);

    CommandStatus execute() override;
    bool parseArgs(const std::vector<std::string>& args) override;

private:
    World& world_;
};

}
