#pragma once

#include "commands/Command.hpp"

namespace Zappy {

class World;

class InventoryCommand : public Command {
public:
    InventoryCommand(World& world, const std::vector<std::string>& tokens, ClientConnection* client);

    CommandStatus execute() override;
    bool parseArgs(const std::vector<std::string>& args) override;

private:
    World& world_;
};

}
