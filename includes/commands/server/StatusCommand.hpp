#pragma once

#include "commands/ServerCommand.hpp"

namespace Zappy {
    class StatusCommand : public ServerCommand {
    public:
        StatusCommand(Engine* engine);
        void execute() override;
        std::string getDescription() const override {
            return "Display server status and statistics";
        }
    };
} 