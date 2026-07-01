#pragma once
#ifndef KAD_EXTRACTORS_EXTRACTOR_BASE_HPP
#define KAD_EXTRACTORS_EXTRACTOR_BASE_HPP

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace kad::extractors {

    /**
     * IExtractor
     * 
     * Interface for all file content extractors.
     */
    class IExtractor {
    public:
        virtual ~IExtractor() = default;

        /** Performs the actual extraction */
        virtual json extract(const std::string& file_path) = 0;

        /** Returns true if this extractor can handle the given MIME type */
        virtual bool canHandle(const std::string& mime_type) const = 0;

        /** Identifier for this extractor plugin */
        virtual std::string name() const = 0;
    };

} // namespace kad::extractors

#endif // KAD_EXTRACTORS_EXTRACTOR_BASE_HPP
