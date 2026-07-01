#pragma once
#ifndef KAD_FBE_EXTRACTOR_HPP
#define KAD_FBE_EXTRACTOR_HPP

#include "fbe_base.hpp"
#include "shared_attribute.hpp"
#include "../extractors/extractor_base.hpp"
#include <nlohmann/json.hpp>
#include <vector>
#include <memory>
#include <string>

using json = nlohmann::json;

namespace kad::fbe {

    /**
     * FBE_Extractor
     * 
     * Fact Base Element responsible for dispatching extraction tasks
     * to the appropriate IExtractor plugin based on MIME type.
     */
    class FBE_Extractor : public FBE_Base {
    public:
        // Input queue (set by Instigation from rl_NewFileDetected)
        SharedAttribute<json> atExtractQueue;
        
        // Output result (monitored by rl_ExtractionComplete)
        SharedAttribute<json> atExtractResult;
        
        // Telemetry state
        SharedAttribute<std::string> atExtractStatus;

    private:
        std::vector<std::unique_ptr<kad::extractors::IExtractor>> extractors_;

        std::string currentISO8601();

    public:
        FBE_Extractor(kad::pon::Notifier* notifier);

        void registerExtractor(std::unique_ptr<kad::extractors::IExtractor> extractor);

        // ═══ Methods ═══
        
        /** 
         * Triggered by PON Actions. Reads atExtractQueue, performs extraction,
         * and writes to atExtractResult.
         */
        void extract();
    };

} // namespace kad::fbe

#endif // KAD_FBE_EXTRACTOR_HPP
