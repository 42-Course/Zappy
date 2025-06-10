#include "commands/server/HelpCommand.hpp"
#include "core/Engine.hpp"
#include <iostream>
#include <iomanip>

namespace Zappy {

HelpCommand::HelpCommand(Engine& engine, 
                       const std::vector<std::string>& tokens,
                       const std::map<std::string, std::pair<std::string, std::string>>& commandInfo,
                       ClientConnection* client)
    : Command("help", client)
    , engine_(engine)
    , commandInfo_(commandInfo) {
    // Skip validation if we have no tokens (direct command creation)
    if (!tokens.empty()) {
        // Skip the first token (command name) when parsing args
        std::vector<std::string> args(tokens.begin() + 1, tokens.end());
        if (!parseArgs(args)) {
            setErrorMessage("Help command takes at most one argument");
            setStatus(CommandStatus::INVALID);
        }
    }
}

CommandStatus HelpCommand::execute() {
    if (getStatus() == CommandStatus::INVALID) {
        std::cout << "Error: " << getErrorMessage() << "\n";
        std::cout << "Usage: " << getUsage() << "\n";
        return CommandStatus::FAILED;
    }

    // If specific command help was requested
    if (!args_.empty()) {
        const std::string& targetCommand = args_[0];
        auto it = commandInfo_.find(targetCommand);
        if (it != commandInfo_.end()) {
            std::cout << it->first << " - " << it->second.first << "\n";
            std::cout << "Usage: " << it->second.second << "\n";
        } else {
            std::cout << "Command '" << targetCommand << "' not found.\n";
            return CommandStatus::FAILED;
        }
        return CommandStatus::COMPLETED;
    }

    // Display help for all commands
    std::cout << "Available commands:\n";
    for (const auto& [cmd, info] : commandInfo_) {
        std::cout << std::setw(15) << std::left << cmd << " - " << info.first << "\n";
    }
    
    std::cout << "\nType 'help <command>' for detailed usage information.\n";
    return CommandStatus::COMPLETED;
}

bool HelpCommand::parseArgs(const std::vector<std::string>& args) {
    // Clear previous arguments
    args_.clear();
    
    // Help can take 0 or 1 argument
    if (args.size() > 1) {
        return false;
    }
    
    // Store arguments if any
    args_ = args;
    return true;
}

std::string HelpCommand::getDescription() const {
    return "Display available commands and their usage";
}

std::string HelpCommand::getUsage() const {
    return "help [command] - Show help for all commands or specific command";
}

} // namespace Zappy 