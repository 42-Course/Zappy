#pragma once

#include "commands/Command.hpp"
#include "core/World.hpp"

namespace Zappy {

class RightCommand : public Command {
public:
    RightCommand(World& world, const std::vector<std::string>& tokens, ClientConnection* client);
    bool parseArgs(const std::vector<std::string>& args) override;
    CommandStatus execute() override;

private:
    World& world_;
};

}
