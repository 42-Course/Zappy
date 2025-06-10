#pragma once

#include "commands/ICommand.hpp"
#include "net/ClientConnection.hpp"
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace Zappy {

class CommandFactory {
public:
    using CommandCreator = std::function<CommandPtr(ClientConnection*, const std::vector<std::string>&)>;
    
    static CommandFactory& getInstance();
    
    // Register a command creator function
    void registerCommand(const std::string& name, CommandType type, CommandCreator creator);
    
    // Create a command instance
    CommandPtr createCommand(const std::string& name, CommandType type,
                           ClientConnection* client, const std::vector<std::string>& args);
    
    // Get all registered commands of a specific type
    std::vector<CommandPtr> getRegisteredCommands(CommandType type);
    
private:
    CommandFactory() = default;
    ~CommandFactory() = default;
    CommandFactory(const CommandFactory&) = delete;
    CommandFactory& operator=(const CommandFactory&) = delete;
    
    // Commands organized by type and name
    std::map<std::pair<std::string, CommandType>, CommandCreator> commands_;
};

} // namespace Zappy
