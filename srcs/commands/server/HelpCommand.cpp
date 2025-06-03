#include "commands/server/HelpCommand.hpp"
#include "core/Engine.hpp"
#include <iostream>
#include <iomanip>

namespace Zappy {
    HelpCommand::HelpCommand(Engine* engine)
        : ServerCommand("help", engine) {
    }

    void HelpCommand::execute() {
        std::cout << "\nAvailable server commands:\n";
        std::cout << "------------------------\n";
        
        // Format: command - description
        const int cmdWidth = 15;  // Width for command column
        
        std::cout << std::left << std::setw(cmdWidth) << "help"
                  << " - Display this help message\n";
                  
        std::cout << std::left << std::setw(cmdWidth) << "status"
                  << " - Show server status and statistics\n";
                  
        std::cout << std::left << std::setw(cmdWidth) << "exit"
                  << " - Stop the server and exit\n";
                  
        std::cout << std::endl;
    }
} 