#include "extractor_base.hpp"

namespace kad::extractors {

    class AudioExtractor : public IExtractor {
    public:
        std::string name() const override { return "audio_extractor"; }

        bool canHandle(const std::string& mime_type) const override {
            return mime_type == "audio/mpeg" || mime_type == "audio/wav" || mime_type == "audio/ogg";
        }

        json extract(const std::string& file_path) override {
            // Dummy implementation for audio. 
            // Real implementation would invoke whisper.cpp or ffmpeg depending on the engine.
            
            std::string mock_result = "[Audio Transcription Placeholder for: " + file_path + "]";

            return {
                {"extractor", name()},
                {"content_type", "text/plain"},
                {"text", mock_result},
                {"metadata", {
                    {"engine", "whisper_placeholder"}
                }},
                {"confidence", 0.0}
            };
        }
    };

} // namespace kad::extractors
