#include "notifier.hpp"
#include <sys/epoll.h>
#include <sys/signalfd.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>

namespace kad::pon {

    void notifyAttributeChanged(Notifier* n, const std::string& name) {
        if (n) {
            n->onAttributeChanged(name);
        }
    }

    Notifier::Notifier() : running_(false) {
        epoll_fd_ = epoll_create1(EPOLL_CLOEXEC);
        if (epoll_fd_ == -1) {
            throw std::runtime_error("Failed to create epoll file descriptor");
        }
        setupSignalHandling();
    }

    Notifier::~Notifier() {
        if (signal_fd_ != -1) close(signal_fd_);
        if (epoll_fd_ != -1) close(epoll_fd_);
    }

    void Notifier::setupSignalHandling() {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGINT);
        sigaddset(&mask, SIGTERM);

        // Block signals so they are not handled by default handlers
        if (sigprocmask(SIG_BLOCK, &mask, nullptr) == -1) {
            throw std::runtime_error("Failed to block signals");
        }

        // Create a signalfd
        signal_fd_ = signalfd(-1, &mask, SFD_NONBLOCK | SFD_CLOEXEC);
        if (signal_fd_ == -1) {
            throw std::runtime_error("Failed to create signalfd");
        }

        // Register the signal_fd_ in our epoll loop
        registerFd(signal_fd_, [this]() {
            struct signalfd_siginfo fdsi;
            ssize_t s = read(this->signal_fd_, &fdsi, sizeof(struct signalfd_siginfo));
            if (s == sizeof(struct signalfd_siginfo)) {
                if (fdsi.ssi_signo == SIGINT || fdsi.ssi_signo == SIGTERM) {
                    std::cout << "Shutdown signal received. Stopping Notifier loop." << std::endl;
                    this->stop();
                }
            }
        });
    }

    void Notifier::registerRule(const Rule& rule) {
        rules_.push_back(rule);
    }

    void Notifier::onAttributeChanged(const std::string& attribute_name) {
        // In a highly optimized PON, we would map attribute_name directly to dependent Rules
        // For simplicity in this implementation, we evaluate all rules that might care
        
        bool rule_fired = false;
        
        // Important: Use a copy or snapshot if Rules can modify the rule list itself
        // (Not typical in strict PON, but safe).
        for (const auto& rule : rules_) {
            // Check if rule depends on this attribute (optimization)
            bool depends = false;
            for (const auto& premise : rule.premises()) {
                if (premise.targetAttribute() == attribute_name) {
                    depends = true;
                    break;
                }
            }

            if (depends && rule.isReady()) {
                rule.execute();
                rule_fired = true;
            }
        }
        
        // Cascading evaluations are naturally handled because execute() 
        // will call Instigations that call set(), which re-enters onAttributeChanged().
    }

    void Notifier::registerFd(int fd, std::function<void()> handler) {
        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLET; // Edge-triggered
        ev.data.fd = fd;
        
        if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev) == -1) {
            throw std::runtime_error("Failed to add fd to epoll");
        }
        fd_handlers_[fd] = std::move(handler);
    }

    void Notifier::unregisterFd(int fd) {
        epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr);
        fd_handlers_.erase(fd);
    }

    void Notifier::run() {
        running_ = true;
        constexpr int MAX_EVENTS = 10;
        struct epoll_event events[MAX_EVENTS];

        std::cout << "Notifier event loop started. Zero idle CPU." << std::endl;

        while (running_) {
            // Blocks completely until an event happens (inotify, mqtt socket, or signal)
            int n = epoll_wait(epoll_fd_, events, MAX_EVENTS, -1);
            
            if (n == -1) {
                if (errno == EINTR) continue; // Interrupted by non-blocked signal
                std::cerr << "epoll_wait error" << std::endl;
                break;
            }

            for (int i = 0; i < n; ++i) {
                int active_fd = events[i].data.fd;
                if (fd_handlers_.count(active_fd)) {
                    fd_handlers_[active_fd](); // Dispatch to handler (e.g. inotify watcher)
                }
            }
        }
        
        std::cout << "Notifier event loop stopped." << std::endl;
    }

    void Notifier::stop() {
        running_ = false;
    }

} // namespace kad::pon
