#include "net/EventLoop.hpp"
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <unistd.h>

namespace Zappy {

EventLoop::EventLoop() {
  epollFd_ = epoll_create1(0);
  if (epollFd_ == -1) {
    throw std::runtime_error("Failed to create epoll instance");
  }
}

EventLoop::~EventLoop() {
  if (epollFd_ != -1) {
    close(epollFd_);
  }
}

void EventLoop::addFd(int fd, uint32_t events, std::function<void(uint32_t)> handler) {
  epoll_event ev;
  ev.events  = events;
  ev.data.fd = fd;

  if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &ev) == -1) {
    throw std::runtime_error("Failed to add fd to epoll: " + std::string(std::strerror(errno)));
  }

  handlers_[fd] = std::move(handler);
}

void EventLoop::removeFd(int fd) {
  if (epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, nullptr) == -1) {
    std::cerr << "Warning: failed to remove fd " << fd << " from epoll: " << std::strerror(errno)
              << std::endl;
  }
  handlers_.erase(fd);
}

void EventLoop::poll(int timeoutMs) {
  epoll_event events[MAX_EVENTS];

  int nfds = epoll_wait(epollFd_, events, MAX_EVENTS, timeoutMs);
  if (nfds == -1) {
    if (errno != EINTR) {
      throw std::runtime_error("epoll_wait failed: " + std::string(std::strerror(errno)));
    }
    return;
  }

  for (int i = 0; i < nfds; ++i) {
    int fd           = events[i].data.fd;
    uint32_t revents = events[i].events;

    auto it = handlers_.find(fd);
    if (it != handlers_.end() && it->second) {
      it->second(revents);
    }
  }
}

} // namespace Zappy
