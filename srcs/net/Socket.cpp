#include "net/Socket.hpp"
#include <fcntl.h>
#include <netinet/in.h>
#include <stdexcept>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

namespace Zappy {

Socket::Socket(int domain, int type, int protocol) : fd_(-1) {
  fd_ = socket(domain, type, protocol);
  if (fd_ == -1) {
    throw std::runtime_error("Failed to create socket: " + std::string(strerror(errno)));
  }

  // Allow reuse of address
  int opt = 1;
  if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
    close(fd_);
    throw std::runtime_error("Failed to set socket options: " + std::string(strerror(errno)));
  }
  setNonBlocking(fd_);
}

Socket::~Socket() {
  if (fd_ != -1) {
    close(fd_);
  }
}

Socket::Socket(Socket &&other) noexcept : fd_(other.fd_) {
  other.fd_ = -1; // Prevent other from closing our fd
}

Socket &Socket::operator=(Socket &&other) noexcept {
  if (this != &other) {
    if (fd_ != -1) {
      close(fd_);
    }
    fd_       = other.fd_;
    other.fd_ = -1;
  }
  return *this;
}

void Socket::setNonBlocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags == -1) {
    throw std::runtime_error("Failed to get socket flags: " + std::string(strerror(errno)));
  }

  if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
    throw std::runtime_error("Failed to set socket non-blocking: " + std::string(strerror(errno)));
  }
}

} // namespace Zappy