#pragma once

namespace Zappy {
    // Helper class for RAII socket handling
    class Socket {
    public:
        Socket(int domain, int type, int protocol);
        ~Socket();
        
        int getFd() const { return fd_; }
        
        // Prevent copying
        Socket(const Socket&) = delete;
        Socket& operator=(const Socket&) = delete;
        
        // Allow moving
        Socket(Socket&& other) noexcept;
        Socket& operator=(Socket&& other) noexcept;
        
        // Static methods
        static void setNonBlocking(int fd);
    private:
        int fd_;
    };
} 