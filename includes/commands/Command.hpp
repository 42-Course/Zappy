#pragma once

#include <string>

namespace Zappy {
    class Command {
    public:
        explicit Command(const std::string& name) : name_(name) {}
        virtual ~Command() = default;

        // Pure virtual method that each command must implement
        virtual void execute() = 0;

        // Getters
        const std::string& getName() const { return name_; }

    protected:
        std::string name_;
    };
} 