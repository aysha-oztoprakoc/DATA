#pragma once
#ifndef KAD_FBE_TELL_STORAGE_HPP
#define KAD_FBE_TELL_STORAGE_HPP

#include "fbe_base.hpp"
#include "shared_attribute.hpp"
#include "../io/inotify_watcher.hpp"
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace kad::fbe {

    /**
     * FBE_TellStorage
     * 
     * Fact Base Element representing the storage subsystem of the TELL node.
     * Integrates with InotifyWatcher to populate attributes when files arrive.
     */
    class FBE_TellStorage : public FBE_Base {
    public:
        SharedAttribute<json> atFileNew;
        SharedAttribute<int> atFileCount;
        SharedAttribute<std::string> atVectorized;
        SharedAttribute<json> atStorageHealth;

    private:
        kad::io::InotifyWatcher watcher_;
        std::string inbox_path_;
        std::string processed_path_;

        // Helper methods
        size_t getFileSize(const std::string& path);
        std::string detectMimeType(const std::string& path);
        std::string computeSHA256(const std::string& path);
        std::string currentISO8601();

    public:
        FBE_TellStorage(kad::pon::Notifier* notifier, const std::string& inbox, const std::string& processed);

        /**
         * Returns the fd of the inotify watcher so it can be registered with epoll.
         */
        int getWatcherFd() const;
        void processWatcherEvents();

        // ═══ Methods ═══
        
        /** Move file to processed directory */
        void moveToProcessed(const std::string& file_hash);
    };

} // namespace kad::fbe

#endif // KAD_FBE_TELL_STORAGE_HPP
