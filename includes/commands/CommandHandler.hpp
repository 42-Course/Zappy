#pragma once

#include "commands/CommandFactory.hpp"
#include "services/Logger.hpp"
#include <queue>
#include <mutex>

namespace Zappy {

class CommandHandler {
public:
    CommandHandler() = default;
    
    // Add a command to the queue
    void queueCommand(CommandPtr command);
    
    // Process commands in the queue
    void processCommands();
    
    // Check if there are commands in the queue
    bool hasCommands() const;
    
    // Get number of commands in queue
    size_t queueSize() const;

private:
    bool isCommandReady(const CommandPtr& command);
    
    std::queue<CommandPtr> commandQueue_;
    mutable std::mutex queueMutex_;
};

} // namespace Zappy 