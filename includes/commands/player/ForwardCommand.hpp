#pragma once

#include "commands/Command.hpp"
#include "core/World.hpp"

namespace Zappy {

class ForwardCommand : public Command {
public:
    ForwardCommand(World& world, const std::vector<std::string>& tokens, ClientConnection* client);

    CommandStatus execute() override;
    bool parseArgs(const std::vector<std::string>& args) override;

private:
    World& world_;
};

}
