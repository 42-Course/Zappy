#pragma once

#include "commands/Command.hpp"
#include "net/ClientConnection.hpp"
#include "core/World.hpp"

namespace Zappy {

class World;

class BCTCommand : public Command {
public:
    BCTCommand(World& world, const std::vector<std::string>& tokens, ClientConnection* client);
    CommandStatus execute() override;
    bool parseArgs(const std::vector<std::string>& args) override;

private:
    World& world_;
    int x_;
    int y_;
};

}  // namespace Zappy
