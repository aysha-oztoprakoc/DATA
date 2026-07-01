#include "extractor_base.hpp"
#include <fstream>
#include <sstream>

namespace kad::extractors {

    class TextExtractor : public IExtractor {
    public:
        std::string name() const override { return "text_extractor"; }

        bool canHandle(const std::string& mime_type) const override {
            return mime_type == "text/plain" || mime_type == "text/markdown" || mime_type == "text/csv";
        }

        json extract(const std::string& file_path) override {
            std::ifstream file(file_path);
            if (!file) {
                throw std::runtime_error("Could not open text file.");
            }
            std::stringstream buffer;
            buffer << file.rdbuf();

            return {
                {"extractor", name()},
                {"content_type", "text/plain"},
                {"text", buffer.str()},
                {"metadata", json::object()},
                {"confidence", 1.0}
            };
        }
    };

} // namespace kad::extractors
