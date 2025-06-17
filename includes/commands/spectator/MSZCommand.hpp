#pragma once

#include "commands/Command.hpp"
#include "net/ClientConnection.hpp"

namespace Zappy {

class World;  // Forward declaration

class MSZCommand : public Command {
public:
    MSZCommand( World& world, const std::vector<std::string>& tokens, ClientConnection* client);
    
    CommandStatus execute() override;
    bool parseArgs(const std::vector<std::string>& args) override;

private:
    World& world_;
};

} // namespace Zappy 