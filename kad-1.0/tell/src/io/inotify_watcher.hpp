#pragma once
#ifndef KAD_IO_INOTIFY_WATCHER_HPP
#define KAD_IO_INOTIFY_WATCHER_HPP

#include <sys/inotify.h>
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>

namespace kad::io {

    /**
     * InotifyWatcher
     * 
     * Provides a zero-polling interface to Linux filesystem events.
     * Generates a file descriptor that can be monitored via epoll.
     */
    class InotifyWatcher {
    private:
        int inotify_fd_;
        std::unordered_map<int, std::function<void(const inotify_event&, const std::string&)>> callbacks_;
        std::unordered_map<int, std::string> wd_to_path_; // Maps watch descriptor to base path

    public:
        InotifyWatcher();
        ~InotifyWatcher();

        /**
         * Adds a watch for a directory.
         * The callback receives the event and the base directory path.
         */
        int watch(const std::string& path, uint32_t mask, 
                  std::function<void(const inotify_event&, const std::string&)> callback);

        void unwatch(int wd);

        /**
         * Returns the file descriptor to be registered with epoll.
         */
        int fd() const;

        /**
         * Reads available events from the fd and dispatches callbacks.
         */
        void processEvents();
    };

} // namespace kad::io

#endif // KAD_IO_INOTIFY_WATCHER_HPP
