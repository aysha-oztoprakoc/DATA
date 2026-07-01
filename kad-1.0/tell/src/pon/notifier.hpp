#pragma once
#ifndef KAD_PON_NOTIFIER_HPP
#define KAD_PON_NOTIFIER_HPP

#include "rule.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

namespace kad::pon {

    /**
     * Notifier
     * 
     * The central engine of the Notification-Oriented Paradigm.
     * Manages rules and the epoll event loop.
     */
    class Notifier {
    private:
        std::vector<Rule> rules_;
        int epoll_fd_;
        int signal_fd_;
        bool running_;
        
        // Maps file descriptors to their handlers (for inotify, mqtt socket, etc)
        std::unordered_map<int, std::function<void()>> fd_handlers_;

        void setupSignalHandling();

    public:
        Notifier();
        ~Notifier();

        void registerRule(const Rule& rule);
        void onAttributeChanged(const std::string& attribute_name);
        
        void registerFd(int fd, std::function<void()> handler);
        void unregisterFd(int fd);

        void run();
        void stop();
    };

    // Forward declaration implementation for SharedAttribute
    void notifyAttributeChanged(Notifier* n, const std::string& name);

} // namespace kad::pon

#endif // KAD_PON_NOTIFIER_HPP
