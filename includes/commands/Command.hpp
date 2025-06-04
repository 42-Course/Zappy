#pragma once

#include <string>
#include <vector>

namespace Zappy {
    class Command {
    public:
        explicit Command(const std::string& commandLine) {
            // Parse command line into name and args
            size_t pos = commandLine.find(' ');
            if (pos != std::string::npos) {
                name_ = commandLine.substr(0, pos);
                // Parse remaining arguments
                std::string remaining = commandLine.substr(pos + 1);
                size_t start = 0, end;
                while ((end = remaining.find(' ', start)) != std::string::npos) {
                    if (end > start) {
                        args_.push_back(remaining.substr(start, end - start));
                    }
                    start = end + 1;
                }
                if (start < remaining.length()) {
                    args_.push_back(remaining.substr(start));
                }
            } else {
                name_ = commandLine;
            }
        }
        virtual ~Command() = default;

        // Pure virtual method that each command must implement
        virtual void execute() = 0;

        // Getters
        const std::string& getName() const { return name_; }
        const std::vector<std::string>& getArgs() const { return args_; }

    protected:
        std::string name_;
        std::vector<std::string> args_;
    };
} 