#include "fbe_tell_storage.hpp"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <openssl/evp.h>

namespace fs = std::filesystem;

namespace kad::fbe {

    FBE_TellStorage::FBE_TellStorage(kad::pon::Notifier* notifier, const std::string& inbox, const std::string& processed)
        : FBE_Base("TellStorage", notifier)
        , atFileNew("TellStorage.fileNew", json::object(), notifier)
        , atFileCount("TellStorage.fileCount", 0, notifier)
        , atVectorized("TellStorage.vectorized", "", notifier)
        , atStorageHealth("TellStorage.storageHealth", json::object(), notifier)
        , inbox_path_(inbox)
        , processed_path_(processed)
    {
        // Ensure directories exist
        fs::create_directories(inbox_path_ + "/images");
        fs::create_directories(inbox_path_ + "/audio");
        fs::create_directories(inbox_path_ + "/text");
        fs::create_directories(inbox_path_ + "/documents");
        fs::create_directories(inbox_path_ + "/misc");
        fs::create_directories(processed_path_);

        auto handle_event = [this](const inotify_event& event, const std::string& base_path) {
            if (event.len > 0) {
                std::string filename(event.name);
                std::string full_path = base_path + "/" + filename;
                
                if (filename[0] == '.' || fs::is_directory(full_path)) return;

                json file_info = {
                    {"path", full_path},
                    {"name", filename},
                    {"size_bytes", this->getFileSize(full_path)},
                    {"mime_type", this->detectMimeType(full_path)},
                    {"sha256", this->computeSHA256(full_path)},
                    {"timestamp", this->currentISO8601()}
                };
                
                std::string event_type = (event.mask & IN_CLOSE_WRITE) ? "IN_CLOSE_WRITE" : "IN_MOVED_TO";
                
                json ingest_event = {
                    {"event_id", "TODO-UUID"},
                    {"timestamp", this->currentISO8601()},
                    {"source_node", "tell"},
                    {"file", file_info},
                    {"inotify_event", event_type}
                };

                this->atFileNew.set(ingest_event);
                this->atFileCount.set(this->atFileCount.get() + 1);
            }
        };

        // Watch root inbox
        watcher_.watch(inbox_path_, IN_CLOSE_WRITE | IN_MOVED_TO, handle_event);
        
        // Also watch subdirectories
        for (const auto& entry : fs::directory_iterator(inbox_path_)) {
            if (entry.is_directory()) {
                watcher_.watch(entry.path().string(), IN_CLOSE_WRITE | IN_MOVED_TO, handle_event);
            }
        }
    }

    int FBE_TellStorage::getWatcherFd() const {
        return watcher_.fd();
    }

    void FBE_TellStorage::processWatcherEvents() {
        watcher_.processEvents();
    }

    void FBE_TellStorage::moveToProcessed(const std::string& file_hash) {
        // Find file with matching hash in inbox...
        // For efficiency, we should maintain a hash -> path mapping, 
        // but for now we search or assume the file is passed directly.
        // Simplifying for this implementation:
        
        // Find the matching file in the inbox
        for (const auto& entry : fs::recursive_directory_iterator(inbox_path_)) {
            if (entry.is_regular_file()) {
                if (computeSHA256(entry.path().string()) == file_hash) {
                    std::string target = processed_path_ + "/" + entry.path().filename().string();
                    fs::rename(entry.path(), target);
                    return;
                }
            }
        }
    }

    size_t FBE_TellStorage::getFileSize(const std::string& path) {
        try {
            return fs::file_size(path);
        } catch (...) {
            return 0;
        }
    }

    std::string FBE_TellStorage::detectMimeType(const std::string& path) {
        // Basic mapping for MVP
        std::string ext = fs::path(path).extension().string();
        if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
        if (ext == ".png") return "image/png";
        if (ext == ".pdf") return "application/pdf";
        if (ext == ".txt" || ext == ".md") return "text/plain";
        if (ext == ".mp3") return "audio/mpeg";
        return "application/octet-stream";
    }

    std::string FBE_TellStorage::computeSHA256(const std::string& path) {
        // Note: Using OpenSSL EVP
        std::ifstream file(path, std::ios::binary);
        if (!file) return "";

        EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
        EVP_DigestInit_ex(mdctx, EVP_sha256(), nullptr);

        char buffer[8192];
        while (file.read(buffer, sizeof(buffer))) {
            EVP_DigestUpdate(mdctx, buffer, file.gcount());
        }
        EVP_DigestUpdate(mdctx, buffer, file.gcount()); // Final read

        unsigned char hash[EVP_MAX_MD_SIZE];
        unsigned int lengthOfHash = 0;
        EVP_DigestFinal_ex(mdctx, hash, &lengthOfHash);
        EVP_MD_CTX_free(mdctx);

        std::stringstream ss;
        for (unsigned int i = 0; i < lengthOfHash; ++i) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }
        return ss.str();
    }

    std::string FBE_TellStorage::currentISO8601() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::gmtime(&in_time_t), "%Y-%m-%dT%H:%M:%SZ");
        return ss.str();
    }

} // namespace kad::fbe
