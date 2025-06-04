#pragma once

#include "commands/ServerCommand.hpp"
#include <map>

namespace Zappy {

class HelpCommand : public ServerCommand {
public:
    HelpCommand(Engine& engine, const std::string& commandLine,
                const std::map<std::string, std::pair<std::string, std::string>>& commands);
    ~HelpCommand() = default;

    void execute() override;
    std::string getDescription() const override;
    std::string getUsage() const override;

    // Static command registry
    static void registerCommand(const std::string& name, const std::string& description, const std::string& usage);
    static const std::map<std::string, std::pair<std::string, std::string>>& getCommands();

private:
    const std::map<std::string, std::pair<std::string, std::string>>& commands_;
};

} // namespace Zappy 