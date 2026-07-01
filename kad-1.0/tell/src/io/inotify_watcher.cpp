#include "inotify_watcher.hpp"
#include <unistd.h>
#include <stdexcept>
#include <iostream>

namespace kad::io {

    InotifyWatcher::InotifyWatcher() {
        inotify_fd_ = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
        if (inotify_fd_ == -1) {
            throw std::runtime_error("Failed to initialize inotify");
        }
    }

    InotifyWatcher::~InotifyWatcher() {
        if (inotify_fd_ != -1) {
            close(inotify_fd_);
        }
    }

    int InotifyWatcher::watch(const std::string& path, uint32_t mask, 
                              std::function<void(const inotify_event&, const std::string&)> callback) {
        int wd = inotify_add_watch(inotify_fd_, path.c_str(), mask);
        if (wd == -1) {
            throw std::runtime_error("Failed to add inotify watch for path: " + path);
        }
        
        callbacks_[wd] = std::move(callback);
        wd_to_path_[wd] = path;
        
        std::cout << "Added inotify watch for " << path << " (wd=" << wd << ")" << std::endl;
        return wd;
    }

    void InotifyWatcher::unwatch(int wd) {
        inotify_rm_watch(inotify_fd_, wd);
        callbacks_.erase(wd);
        wd_to_path_.erase(wd);
    }

    int InotifyWatcher::fd() const {
        return inotify_fd_;
    }

    void InotifyWatcher::processEvents() {
        // Buffer to hold inotify events (aligned properly)
        char buffer[4096] __attribute__((aligned(__alignof__(struct inotify_event))));
        
        ssize_t len = read(inotify_fd_, buffer, sizeof(buffer));
        if (len <= 0) {
            if (errno != EAGAIN) {
                std::cerr << "Error reading inotify events" << std::endl;
            }
            return;
        }

        const struct inotify_event* event;
        for (char* ptr = buffer; ptr < buffer + len; ptr += sizeof(struct inotify_event) + event->len) {
            event = reinterpret_cast<const struct inotify_event*>(ptr);
            
            if (event->len > 0 && callbacks_.count(event->wd)) {
                // Dispatch event with its base path
                callbacks_[event->wd](*event, wd_to_path_[event->wd]);
            }
        }
    }

} // namespace kad::io
