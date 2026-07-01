#include "fbe_extractor.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>

namespace kad::fbe {

    FBE_Extractor::FBE_Extractor(kad::pon::Notifier* notifier)
        : FBE_Base("Extractor", notifier)
        , atExtractQueue("Extractor.extractQueue", json::object(), notifier)
        , atExtractResult("Extractor.extractResult", json::object(), notifier)
        , atExtractStatus("Extractor.extractStatus", "idle", notifier)
    {}

    void FBE_Extractor::registerExtractor(std::unique_ptr<kad::extractors::IExtractor> extractor) {
        if (extractor) {
            std::cout << "Registered extractor: " << extractor->name() << std::endl;
            extractors_.push_back(std::move(extractor));
        }
    }

    void FBE_Extractor::extract() {
        json queue_item = atExtractQueue.get();
        if (queue_item.empty() || !queue_item.contains("file")) {
            return;
        }

        atExtractStatus.set("extracting");

        std::string mime_type = queue_item["file"]["mime_type"];
        std::string path = queue_item["file"]["path"];
        
        std::cout << "Extracting content from: " << path << " (" << mime_type << ")" << std::endl;

        kad::extractors::IExtractor* selected = nullptr;
        for (const auto& ext : extractors_) {
            if (ext->canHandle(mime_type)) {
                selected = ext.get();
                break;
            }
        }

        json extraction_data;
        
        auto start_time = std::chrono::steady_clock::now();

        if (selected) {
            try {
                extraction_data = selected->extract(path);
            } catch (const std::exception& e) {
                extraction_data = {
                    {"extractor", selected->name()},
                    {"content_type", "text/plain"},
                    {"text", std::string("EXTRACTION_ERROR: ") + e.what()},
                    {"metadata", json::object()},
                    {"confidence", 0.0},
                    {"duration_ms", 0}
                };
            }
        } else {
            extraction_data = {
                {"extractor", "fallback_extractor"},
                {"content_type", "text/plain"},
                {"text", "UNSUPPORTED_MIME_TYPE"},
                {"metadata", json::object()},
                {"confidence", 0.0},
                {"duration_ms", 0}
            };
        }

        auto end_time = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        extraction_data["duration_ms"] = duration;

        json result_event = {
            {"event_id", "TODO-UUID"}, // Will generate UUID in final implementation
            {"source_event_id", queue_item.value("event_id", "")},
            {"timestamp", currentISO8601()},
            {"source_node", "tell"},
            {"source_file", queue_item["file"]},
            {"extraction", extraction_data}
        };

        // This mutation triggers rl_ExtractionComplete
        atExtractResult.set(result_event);
        atExtractStatus.set("idle");
    }

    std::string FBE_Extractor::currentISO8601() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::gmtime(&in_time_t), "%Y-%m-%dT%H:%M:%SZ");
        return ss.str();
    }

} // namespace kad::fbe
