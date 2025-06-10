#pragma once

#include "commands/Command.hpp"
#include "net/ClientConnection.hpp"
#include <string>

namespace Zappy {
    class Engine; // Forward declare Engine inside Zappy namespace

    class ClearCommand : public Command {
    public:
        ClearCommand(Engine& engine, 
                    const std::vector<std::string>& tokens,
                    ClientConnection* client);
        
        CommandStatus execute() override;
        bool parseArgs(const std::vector<std::string>& args) override;
        
        std::string getDescription() const override;        
        std::string getUsage() const override;        
    private:
        Engine& engine_;
    };
} // namespace Zappy 