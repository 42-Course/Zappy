#pragma once

#include "commands/ServerCommand.hpp"

namespace Zappy {
    class StatusCommand : public ServerCommand {
    public:
        explicit StatusCommand(Engine& engine);
        void execute() override;
        std::string getDescription() const override {
            return "Display server status and statistics";
        }
        std::string getUsage() const override {
            return "status - Show current server state";
        }
    };
} 