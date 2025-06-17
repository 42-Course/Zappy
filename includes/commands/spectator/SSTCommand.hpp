#pragma once

#include "commands/Command.hpp"
#include "core/GameLoop.hpp"

namespace Zappy {

class SSTCommand : public Command {
public:
    SSTCommand(GameLoop& gameLoop, const std::vector<std::string>& tokens, ClientConnection* client);
    
    CommandStatus execute() override;
    bool parseArgs(const std::vector<std::string>& args) override;

private:
    GameLoop& gameLoop_;
    int tickRate_;
};

} // namespace Zappy
