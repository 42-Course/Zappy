#pragma once

#include "commands/Command.hpp"
#include "net/ClientConnection.hpp"

namespace Zappy {

class GameLoop;

class SGTCommand : public Command {
public:
    SGTCommand(GameLoop& gameLoop, const std::vector<std::string>& tokens, ClientConnection* client);

    CommandStatus execute() override;
    bool parseArgs(const std::vector<std::string>& args) override;

private:
    GameLoop& gameLoop_;
};

}
