#pragma once

#include "commands/Command.hpp"
#include "net/ClientConnection.hpp"
#include <string>
#include <map>

namespace Zappy {
    class Engine; // Forward declare Engine inside Zappy namespace

    class HelpCommand : public Command {
    public:
        HelpCommand(Engine& engine, 
                   const std::vector<std::string>& tokens,
                   const std::map<std::string, std::pair<std::string, std::string>>& commandInfo,
                   ClientConnection* client);
        
        CommandStatus execute() override;
        
        std::string getDescription() const override;
        std::string getUsage() const override;
        bool parseArgs(const std::vector<std::string>& args) override;

    private:
        Engine& engine_;
        const std::map<std::string, std::pair<std::string, std::string>>& commandInfo_;
    };
} // namespace Zappy 