#include "extractor_base.hpp"
#include <cstdio>
#include <memory>
#include <array>
#include <stdexcept>

namespace kad::extractors {

    class ImageExtractor : public IExtractor {
    public:
        std::string name() const override { return "image_extractor"; }

        bool canHandle(const std::string& mime_type) const override {
            return mime_type == "image/jpeg" || mime_type == "image/png" || mime_type == "image/webp";
        }

        json extract(const std::string& file_path) override {
            // Usa tesseract OCR
            // O tesseract guarda o output num ficheiro, ou para stdout com '-'
            std::string cmd = "tesseract '" + file_path + "' stdout -l eng+por --psm 3 2>/dev/null";
            
            std::array<char, 128> buffer;
            std::string result;
            
            std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
            if (!pipe) {
                throw std::runtime_error("popen() failed for tesseract");
            }
            while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
                result += buffer.data();
            }

            return {
                {"extractor", name()},
                {"content_type", "text/plain"},
                {"text", result},
                {"metadata", {
                    {"ocr_engine", "tesseract"}
                }},
                {"confidence", 0.85} // OCR confidence is variable, arbitrary default
            };
        }
    };

} // namespace kad::extractors
