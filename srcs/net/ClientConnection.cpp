#include "net/ClientConnection.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdexcept>

namespace Zappy {
    ClientConnection::ClientConnection(int fd)
        : fd_(fd)
        , type_(Type::Unknown)
        , readBuffer_(BUFFER_SIZE) {
    }

    ClientConnection::~ClientConnection() {
        close();
    }

    bool ClientConnection::readData() {
        ssize_t bytesRead;
        
        do {
            bytesRead = recv(fd_, readBuffer_.data(), BUFFER_SIZE, MSG_DONTWAIT);
            
            if (bytesRead > 0) {
                // Append received data to command buffer
                commandBuffer_.append(readBuffer_.data(), bytesRead);
                processBuffer();
            } else if (bytesRead == 0) {
                // Connection closed by peer
                return false;
            } else if (errno != EAGAIN && errno != EWOULDBLOCK) {
                // Error occurred
                return false;
            }
        } while (bytesRead > 0);

        return true;
    }

    bool ClientConnection::sendData(const std::string& data) {
        size_t totalSent = 0;
        
        while (totalSent < data.length()) {
            ssize_t sent = send(fd_, data.c_str() + totalSent, 
                              data.length() - totalSent, MSG_DONTWAIT);
            
            if (sent > 0) {
                totalSent += sent;
            } else if (sent == -1) {
                if (errno != EAGAIN && errno != EWOULDBLOCK) {
                    return false;
                }
                // Would block, try again later
                break;
            }
        }
        
        return true;
    }

    void ClientConnection::close() {
        if (fd_ != -1) {
            ::close(fd_);
            fd_ = -1;
        }
    }

    void ClientConnection::setType(Type type) {
        type_ = type;
    }

    bool ClientConnection::hasCompleteCommand() const {
        return !pendingCommands_.empty();
    }

    std::string ClientConnection::getNextCommand() {
        if (pendingCommands_.empty()) {
            return "";
        }
        
        std::string cmd = pendingCommands_.front();
        pendingCommands_.pop();
        return cmd;
    }

    void ClientConnection::processBuffer() {
        size_t pos;
        
        // Process all complete commands in the buffer
        while ((pos = commandBuffer_.find('\n')) != std::string::npos) {
            std::string cmd = commandBuffer_.substr(0, pos);
            
            // Remove any carriage return if present
            if (!cmd.empty() && cmd.back() == '\r') {
                cmd.pop_back();
            }
            
            if (!cmd.empty()) {
                pendingCommands_.push(cmd);
            }
            
            // Remove processed command from buffer
            commandBuffer_.erase(0, pos + 1);
        }
        
        // If buffer is too large and no newline found, clear it
        // This prevents memory exhaustion from malicious clients
        if (commandBuffer_.length() > BUFFER_SIZE * 2) {
            commandBuffer_.clear();
        }
    }

    bool ClientConnection::isCommandComplete(const std::string& cmd) const {
        return !cmd.empty() && cmd.back() == '\n';
    }
} 