#pragma once

#include "commands/ServerCommand.hpp"

namespace Zappy {
    class HelpCommand : public ServerCommand {
    public:
        HelpCommand(Engine* engine);
        void execute() override;
        std::string getDescription() const override {
            return "Display available server commands";
        }
    };
} 