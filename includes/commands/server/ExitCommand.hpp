#pragma once

#include "commands/ServerCommand.hpp"
#include "net/ClientConnection.hpp"

namespace Zappy {

class ExitCommand : public ServerCommand {
public:
    ExitCommand(Engine& engine, ClientConnection* client = nullptr);
    
    CommandStatus execute() override;
    bool parseArgs(const std::vector<std::string>& args) override;
    
    std::string getDescription() const override;
    std::string getUsage() const override;
};

} // namespace Zappy 