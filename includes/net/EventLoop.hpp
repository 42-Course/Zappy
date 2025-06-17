#pragma once

#include <functional>
#include <unordered_map>
#include <sys/epoll.h>
#include <cstdint>

namespace Zappy {
    class EventLoop {
    public:
        EventLoop();
        ~EventLoop();

        /**
         * We will register handlers for the following fd's
         * Players Socket (fd) -> Accept players
         * Spectators Socket (fd) -> Accept spectators
         * Stdin Server Commands (STDIO) -> handle messages
         * ClientConnection (FD) -> handle client commands
         */
        void addFd(int fd, uint32_t events, std::function<void(uint32_t)> handler);
        void removeFd(int fd);
        void poll(int timeoutMs = 0);  // non-blocking by default

    private:
        static constexpr int MAX_EVENTS = 64;

        int epollFd_;
        std::unordered_map<int, std::function<void(uint32_t)>> handlers_;
    };
}
