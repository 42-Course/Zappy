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
    
    virtual CommandStatus execute() = 0;

    virtual CommandType getType() const = 0;
    
    virtual std::string getName() const = 0;
    
    virtual int getTimeCost() const = 0;
    
    virtual ClientConnection* getClient() const = 0;
    
    virtual CommandStatus getStatus() const = 0;
    
    virtual void setStatus(CommandStatus status) = 0;
    
    virtual std::string getErrorMessage() const = 0;
    
    virtual bool parseArgs(const std::vector<std::string>& args) = 0;

    virtual std::string getDescription() const = 0;
    
    virtual std::string getUsage() const = 0;
};

using CommandPtr = std::shared_ptr<ICommand>;

} // namespace Zappy 