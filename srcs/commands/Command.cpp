#include "commands/Command.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace Zappy {

Command::Command(const std::string &name, ClientConnection *client)
    : name_(name), status_(CommandStatus::PENDING), type_(CommandType::STDIN), timeCost_(0),
      client_(client), errorMessage_() {}

Command::Command(const std::string &name, CommandType type, int timeCost, ClientConnection *client)
    : name_(name), status_(CommandStatus::PENDING), type_(type), timeCost_(timeCost),
      client_(client), errorMessage_() {}

std::string Command::getName() const {
  return name_;
}

CommandType Command::getType() const {
  return type_;
}

int Command::getTimeCost() const {
  return timeCost_;
}

ClientConnection *Command::getClient() const {
  return client_;
}

CommandStatus Command::getStatus() const {
  return status_;
}

void Command::setStatus(CommandStatus status) {
  status_ = status;
  if (status == CommandStatus::FAILED || status == CommandStatus::INVALID) {
    Logger::getInstance().error("Command " + name_ + " " +
                                (status == CommandStatus::FAILED ? "failed" : "invalid") + ": " +
                                errorMessage_);
  }
}

std::string Command::getErrorMessage() const {
  return errorMessage_;
}

void Command::setErrorMessage(const std::string &message) {
  errorMessage_ = message;
}

void Command::logCommand(const std::string &message) {
  Logger::getInstance().info("Command " + name_ + ": " + message);
}

bool Command::parseArgs(const std::vector<std::string> &rawArgs) {
  // Clear previous arguments
  args_.clear();

  // Make sure we have at least a command name
  if (rawArgs.empty()) {
    setErrorMessage("Empty command");
    return false;
  }

  // Validate command name
  std::string cmdName = trim(rawArgs[0]);
  if (!validateCommandName(cmdName)) {
    return false;
  }

  // Process remaining arguments
  for (size_t i = 1; i < rawArgs.size(); ++i) {
    std::string trimmed = trim(rawArgs[i]);
    if (!trimmed.empty()) { // Skip empty arguments
      args_.push_back(trimmed);
    }
  }

  return true;
}

const std::vector<std::string> &Command::getArgs() const {
  return args_;
}

std::string Command::getArg(size_t index) const {
  if (index < args_.size()) {
    return args_[index];
  }
  return "";
}

size_t Command::getArgCount() const {
  return args_.size();
}

bool Command::validateArgCount(size_t expected, const std::string &usage) {
  if (args_.size() != expected) {
    std::stringstream ss;
    ss << "Expected " << expected << " arguments, got " << args_.size();
    if (!usage.empty()) {
      ss << ". Usage: " << usage;
    }
    setErrorMessage(ss.str());
    return false;
  }
  return true;
}

bool Command::validateArgCount(size_t min, size_t max, const std::string &usage) {
  if (args_.size() < min || args_.size() > max) {
    std::stringstream ss;
    ss << "Expected between " << min << " and " << max << " arguments, got " << args_.size();
    if (!usage.empty()) {
      ss << ". Usage: " << usage;
    }
    setErrorMessage(ss.str());
    return false;
  }
  return true;
}

bool Command::validateCommandName(const std::string &cmdName) {
  std::string trimmedCmd = trim(cmdName);
  if (trimmedCmd != name_) {
    setErrorMessage("Invalid command name: " + cmdName + " (expected: " + name_ + ")");
    return false;
  }
  return true;
}

std::string Command::trim(const std::string &str) {
  // Find first non-whitespace character
  const auto start = str.find_first_not_of(" \t\r\n");
  if (start == std::string::npos) {
    return ""; // String is all whitespace
  }

  // Find last non-whitespace character
  const auto end = str.find_last_not_of(" \t\r\n");

  // Return the trimmed substring
  return str.substr(start, end - start + 1);
}

std::vector<std::string> Command::splitArgs(const std::string &cmdLine) {
  std::vector<std::string> args;
  std::string trimmedLine = trim(cmdLine);

  if (trimmedLine.empty()) {
    return args;
  }

  std::istringstream iss(trimmedLine);
  std::string arg;

  // Split by whitespace while respecting quotes
  bool inQuotes = false;
  std::string currentArg;

  for (char c : trimmedLine) {
    if (c == '"') {
      inQuotes = !inQuotes;
    } else if (std::isspace(c) && !inQuotes) {
      if (!currentArg.empty()) {
        args.push_back(currentArg);
        currentArg.clear();
      }
    } else {
      currentArg += c;
    }
  }

  // Add the last argument if any
  if (!currentArg.empty()) {
    args.push_back(currentArg);
  }

  return args;
}

bool Command::tryParseInt(const std::string &arg, int &out) {
  try {
    std::string trimmed = trim(arg);
    size_t pos;
    out = std::stoi(trimmed, &pos);
    if (pos != trimmed.length()) {
      setErrorMessage("Invalid integer format: " + arg);
      return false;
    }
    return true;
  } catch (const std::exception &e) {
    setErrorMessage("Failed to parse integer: " + arg);
    return false;
  }
}

bool Command::tryParseFloat(const std::string &arg, float &out) {
  try {
    std::string trimmed = trim(arg);
    size_t pos;
    out = std::stof(trimmed, &pos);
    if (pos != trimmed.length()) {
      setErrorMessage("Invalid float format: " + arg);
      return false;
    }
    return true;
  } catch (const std::exception &e) {
    setErrorMessage("Failed to parse float: " + arg);
    return false;
  }
}

bool Command::tryParseCoordinates(size_t startIndex, int &x, int &y) {
  if (startIndex + 1 >= args_.size()) {
    setErrorMessage("Not enough arguments for coordinates");
    return false;
  }

  return tryParseInt(args_[startIndex], x) && tryParseInt(args_[startIndex + 1], y);
}

} // namespace Zappy