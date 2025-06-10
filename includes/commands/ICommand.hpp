#pragma once

#include <string>
#include <vector>
#include <memory>

namespace Zappy {

class ClientConnection; // Forward declaration

enum class CommandType {
    SPECTATOR,
    PLAYER,
    STDIN
};

enum class CommandStatus {
    PENDING,    // Command is waiting to be executed
    EXECUTING,  // Command is currently executing
    COMPLETED,  // Command has finished executing successfully
    FAILED,     // Command failed to execute
    INVALID     // Command is invalid (wrong syntax, wrong state, etc.)
};

class ICommand {
public:
    virtual ~ICommand() = default;
    
    // Execute the command
    virtual CommandStatus execute() = 0;
    
    // Get the command type
    virtual CommandType getType() const = 0;
    
    // Get the command name
    virtual std::string getName() const = 0;
    
    // Get the time cost of the command (in time units)
    virtual int getTimeCost() const = 0;
    
    // Get the client that issued the command
    virtual ClientConnection* getClient() const = 0;
    
    // Get command status
    virtual CommandStatus getStatus() const = 0;
    
    // Set command status
    virtual void setStatus(CommandStatus status) = 0;
    
    // Get error message if status is FAILED or INVALID
    virtual std::string getErrorMessage() const = 0;
    
    // Parse command arguments
    virtual bool parseArgs(const std::vector<std::string>& args) = 0;

    // Get command description for help text
    virtual std::string getDescription() const = 0;
    
    // Get command usage information
    virtual std::string getUsage() const = 0;
};

using CommandPtr = std::shared_ptr<ICommand>;

} // namespace Zappy 