#pragma once

#include "commands/ICommand.hpp"
#include "services/Logger.hpp"
#include "net/ClientConnection.hpp"
#include <sstream>
#include <algorithm>

namespace Zappy {

class Command : public ICommand {
public:
    Command(const std::string& name, ClientConnection* client);
    Command(const std::string& name, CommandType type, int timeCost, ClientConnection* client);
    virtual ~Command() = default;

    // ICommand interface implementation
    std::string getName() const override;
    CommandType getType() const override;
    int getTimeCost() const override;
    ClientConnection* getClient() const override;
    CommandStatus getStatus() const override;
    void setStatus(CommandStatus status) override;
    std::string getErrorMessage() const override;
    bool parseArgs(const std::vector<std::string>& args) override;
    
    // Default implementations for help text
    std::string getDescription() const override { return "No description available"; }
    std::string getUsage() const override { return name_ + " - Basic command"; }

    // Get parsed arguments (excluding command name)
    const std::vector<std::string>& getArgs() const;
    size_t getArgCount() const;
    std::string getArg(size_t index) const;
    
    // String manipulation helpers
    static std::string trim(const std::string& str);
    static std::vector<std::string> splitArgs(const std::string& cmdLine);

protected:
    // Common functionality for derived classes
    void setErrorMessage(const std::string& message);
    void logCommand(const std::string& message);

    // Argument validation helpers
    bool validateArgCount(size_t expected, const std::string& usage = "");
    bool validateArgCount(size_t min, size_t max, const std::string& usage = "");
    
    // Type conversion helpers with validation
    bool tryParseInt(const std::string& arg, int& out);
    bool tryParseFloat(const std::string& arg, float& out);
    bool tryParseCoordinates(size_t startIndex, int& x, int& y);
    
    // Command validation helpers
    bool validateCommandName(const std::string& cmdName);
    
    // Store validated arguments (excluding command name)
    std::vector<std::string> args_;
    std::string name_;
    CommandStatus status_;

private:
    CommandType type_;
    int timeCost_;
    ClientConnection* client_;
    std::string errorMessage_;
};

} // namespace Zappy
 