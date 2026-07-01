#include "extractor_base.hpp"
#include <cstdio>
#include <memory>
#include <array>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

namespace kad::extractors {

    class OfficeExtractor : public IExtractor {
    public:
        std::string name() const override { return "office_extractor"; }

        bool canHandle(const std::string& mime_type) const override {
            return mime_type == "application/vnd.openxmlformats-officedocument.wordprocessingml.document" || 
                   mime_type == "application/msword";
        }

        json extract(const std::string& file_path) override {
            // Create a temp directory for LibreOffice conversion
            std::string tmp_dir = "/tmp/kad_office_" + std::to_string(std::hash<std::string>{}(file_path));
            fs::create_directories(tmp_dir);

            // Convert to txt using headless LibreOffice
            std::string cmd = "libreoffice --headless --convert-to txt:Text --outdir " + tmp_dir + " '" + file_path + "' 2>/dev/null";
            system(cmd.c_str());

            // Read the generated txt file
            std::string original_filename = fs::path(file_path).filename().string();
            std::string txt_filename = original_filename.substr(0, original_filename.find_last_of('.')) + ".txt";
            std::string txt_path = tmp_dir + "/" + txt_filename;

            std::string result = "";
            std::ifstream file(txt_path);
            if (file) {
                std::stringstream buffer;
                buffer << file.rdbuf();
                result = buffer.str();
            }

            // Cleanup
            fs::remove_all(tmp_dir);

            return {
                {"extractor", name()},
                {"content_type", "text/plain"},
                {"text", result},
                {"metadata", json::object()},
                {"confidence", 0.95}
            };
        }
    };

} // namespace kad::extractors
