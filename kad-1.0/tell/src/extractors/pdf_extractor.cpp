#include "extractor_base.hpp"
#include <cstdio>
#include <memory>
#include <array>
#include <stdexcept>

namespace kad::extractors {

    class PdfExtractor : public IExtractor {
    public:
        std::string name() const override { return "pdf_extractor"; }

        bool canHandle(const std::string& mime_type) const override {
            return mime_type == "application/pdf";
        }

        json extract(const std::string& file_path) override {
            // Usa pdftotext (do pacote poppler-utils)
            std::string cmd = "pdftotext -q '" + file_path + "' -";
            
            std::array<char, 128> buffer;
            std::string result;
            
            // popen bloqueia a thread do extrator, mas como a extração deve correr em pool/threads no futuro,
            // ou num FBE assíncrono avançado, por agora assumimos execução síncrona dentro da ação do PON.
            std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
            if (!pipe) {
                throw std::runtime_error("popen() failed for pdftotext");
            }
            while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
                result += buffer.data();
            }

            return {
                {"extractor", name()},
                {"content_type", "text/plain"},
                {"text", result},
                {"metadata", json::object()},
                {"confidence", 0.95} // Arbitrary high confidence for native text
            };
        }
    };

} // namespace kad::extractors
