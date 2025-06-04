#include "commands/server/HelpCommand.hpp"
#include "core/Engine.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>

namespace Zappy {

HelpCommand::HelpCommand(Engine& engine, const std::string& commandLine,
                        const std::map<std::string, std::pair<std::string, std::string>>& commands)
    : ServerCommand(engine, commandLine)
    , commands_(commands)
{}

void HelpCommand::execute() {
    if (!args_.empty()) {
        // Show help for specific command
        auto it = commands_.find(args_[0]);
        if (it != commands_.end()) {
            std::cout << "\n\033[1;34m" << it->first << "\033[0m\n";
            std::cout << "Description: " << it->second.first << "\n";
            std::cout << "Usage: " << it->second.second << "\n\n";
        } else {
            std::cout << "\033[1;31mUnknown command: " << args_[0] << "\033[0m\n";
        }
    } else {
        // Show all commands
        std::cout << "\n\033[1;32m=== Available Commands ===\033[0m\n\n";
        
        // Find the longest command name for padding
        size_t maxLen = 0;
        for (const auto& [name, _] : commands_) {
            maxLen = std::max(maxLen, name.length());
        }
        
        // Print each command with description
        for (const auto& [name, info] : commands_) {
            std::cout << std::left << std::setw(maxLen + 2) << name 
                     << "- " << info.first << "\n";
        }
        std::cout << "\nType 'help <command>' for detailed usage information.\n\n";
    }
}

std::string HelpCommand::getDescription() const {
    return "Display available commands and their usage";
}

std::string HelpCommand::getUsage() const {
    return "help [command] - Show help for all commands or specific command";
}

} // namespace Zappy 