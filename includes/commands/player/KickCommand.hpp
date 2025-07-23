#pragma once

#include "commands/Command.hpp"
#include "core/World.hpp"

namespace Zappy {

class KickCommand : public Command {
public:
    KickCommand(World& world, const std::vector<std::string>& tokens, ClientConnection* client);
    bool parseArgs(const std::vector<std::string>& args) override;
    CommandStatus execute() override;

private:
    World& world_;
};

}
