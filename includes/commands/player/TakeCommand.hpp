#pragma once

#include "commands/Command.hpp"
#include "core/World.hpp"
#include "core/Resource.hpp"

namespace Zappy {

class TakeCommand : public Command {
public:
  TakeCommand(World& world, const std::vector<std::string>& tokens, ClientConnection* client);
  bool parseArgs(const std::vector<std::string>& args) override;
  CommandStatus execute() override;

private:
  World& world_;
  ResourceType type_;
};

}
