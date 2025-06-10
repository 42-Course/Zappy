#pragma once

#include "commands/Command.hpp"
#include "services/WatchService.hpp"
#include "net/ClientConnection.hpp"
#include <string>

namespace Zappy {
    class Engine; // Forward declare Engine inside Zappy namespace

    class WatchCommand : public Command {
    public:
        WatchCommand(Engine& engine, 
                    const std::vector<std::string>& tokens,
                    WatchService& watchService,
                    ClientConnection* client);
        
        CommandStatus execute() override;
        bool parseArgs(const std::vector<std::string>& args) override;
        
        std::string getDescription() const override {
            return "Toggle watch mode to monitor server status";
        }
        
        std::string getUsage() const override {
            return "watch [on|off] - Toggle watch mode or explicitly enable/disable it";
        }
        
    private:
        Engine& engine_;
        WatchService& watchService_;
    };
} // namespace Zappy 