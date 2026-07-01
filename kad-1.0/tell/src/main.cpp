/**
 * main.cpp
 * 
 * Entry point for the KAD TELL node.
 * STRICT PON COMPLIANCE: Assembles the Facto-Executional and Logico-Causal layers.
 * Zero CPU in idle via epoll.
 */

#include "pon/notifier.hpp"
#include "pon/rule.hpp"
#include "pon/premise.hpp"
#include "pon/action.hpp"
#include "pon/instigation.hpp"

#include "fbe/fbe_tell_storage.hpp"
#include "fbe/fbe_extractor.hpp"

#include "extractors/extractor_base.hpp"
#include "extractors/text_extractor.cpp"
#include "extractors/pdf_extractor.cpp"
#include "extractors/image_extractor.cpp"
#include "extractors/audio_extractor.cpp"
#include "extractors/office_extractor.cpp"

#include "io/redis_publisher.hpp"
#include "io/redis_subscriber.hpp"

#include "redis_channels.hpp" // Shared channels

#include <iostream>
#include <cstdlib>
#include <memory>

using namespace kad::pon;
using namespace kad::fbe;
using namespace kad::io;

// Helper to read env vars with defaults
std::string getEnv(const char* name, const char* default_value = "") {
    const char* val = std::getenv(name);
    return val ? std::string(val) : std::string(default_value);
}

int main() {
    std::cout << "Starting TELL node (PON Core with Redis)..." << std::endl;

    // Configuration
    std::string broker_url = "tcp://" + getEnv("MQTT_BROKER", "192.168.0.1") + ":" + getEnv("REDIS_PORT", "6379");
    std::string inbox_path = getEnv("INBOX_PATH", "/srv/kad/inbox");
    std::string processed_path = getEnv("PROCESSED_PATH", "/srv/kad/processed");

    try {
        // 1. Initialize Notifier (the Engine)
        Notifier notifier;

        // 2. Initialize IO Adapters
        RedisPublisher redis_pub(broker_url);
        RedisSubscriber redis_sub(broker_url);
        
        redis_pub.connect();
        redis_sub.connect();

        // 3. Initialize Fact Base Elements (Facto-Executional)
        FBE_TellStorage storage(&notifier, inbox_path, processed_path);
        FBE_Extractor extractor(&notifier);

        // Register Extractors
        extractor.registerExtractor(std::make_unique<kad::extractors::TextExtractor>());
        extractor.registerExtractor(std::make_unique<kad::extractors::PdfExtractor>());
        extractor.registerExtractor(std::make_unique<kad::extractors::ImageExtractor>());
        extractor.registerExtractor(std::make_unique<kad::extractors::AudioExtractor>());
        extractor.registerExtractor(std::make_unique<kad::extractors::OfficeExtractor>());

        // Register Inotify fd with Notifier's epoll
        notifier.registerFd(storage.getWatcherFd(), [&storage]() {
            storage.processWatcherEvents();
        });

        // 4. Define Rules (Logico-Causal)
        
        // Rule 1: rl_NewFileDetected
        Rule rl_NewFileDetected(
            "rl_NewFileDetected",
            {
                Premise("TellStorage.fileNew", PremiseOp::CHANGED, [&]() { return storage.atFileNew.changed() && !storage.atFileNew.get().empty(); })
            },
            {
                std::make_shared<Action>("Publish_Ingest_New", [&]() {
                    redis_pub.publish(kad::redis::TELL_INGEST_NEW, storage.atFileNew.get().dump());
                }),
                std::make_shared<Instigation>("Instigate_Extraction", "Extractor.extractQueue", [&]() {
                    extractor.atExtractQueue.set(storage.atFileNew.get());
                }),
                std::make_shared<Action>("Reset_Attribute", [&]() {
                    storage.atFileNew.resetChanged();
                })
            }
        );

        // Rule 2: rl_ExtractionTrigger
        Rule rl_ExtractionTrigger(
            "rl_ExtractionTrigger",
            {
                Premise("Extractor.extractQueue", PremiseOp::CHANGED, [&]() { return extractor.atExtractQueue.changed(); })
            },
            {
                std::make_shared<Action>("Method_Extract", [&]() {
                    extractor.extract();
                }),
                std::make_shared<Action>("Reset_Attribute", [&]() {
                    extractor.atExtractQueue.resetChanged();
                })
            }
        );

        // Rule 3: rl_ExtractionComplete
        Rule rl_ExtractionComplete(
            "rl_ExtractionComplete",
            {
                Premise("Extractor.extractResult", PremiseOp::CHANGED, [&]() { return extractor.atExtractResult.changed() && !extractor.atExtractResult.get().empty(); })
            },
            {
                std::make_shared<Action>("Publish_Extract_Done", [&]() {
                    redis_pub.publish(kad::redis::TELL_EXTRACT_DONE, extractor.atExtractResult.get().dump());
                }),
                std::make_shared<Action>("Reset_Attribute", [&]() {
                    extractor.atExtractResult.resetChanged();
                })
            }
        );

        // Register Rules
        notifier.registerRule(rl_NewFileDetected);
        notifier.registerRule(rl_ExtractionTrigger);
        notifier.registerRule(rl_ExtractionComplete);

        // 5. Wire Redis Subscriber callbacks to FBEs
        redis_sub.subscribe(kad::redis::AMDY_VECTORIZE_DONE, [&storage](const std::string& payload) {
            try {
                json ack = json::parse(payload);
                if (ack.value("success", false) == true) {
                    std::string hash = ack.value("source_hash", "");
                    storage.atVectorized.set(hash); 
                }
            } catch(...) {}
        });

        // Rule 4: rl_VectorizationConfirmed
        Rule rl_VectorizationConfirmed(
            "rl_VectorizationConfirmed",
            {
                Premise("TellStorage.vectorized", PremiseOp::CHANGED, [&]() { return storage.atVectorized.changed() && !storage.atVectorized.get().empty(); })
            },
            {
                std::make_shared<Action>("Method_MoveToProcessed", [&]() {
                    storage.moveToProcessed(storage.atVectorized.get());
                }),
                std::make_shared<Action>("Reset_Attribute", [&]() {
                    storage.atVectorized.resetChanged();
                })
            }
        );
        notifier.registerRule(rl_VectorizationConfirmed);

        // 6. Run Engine
        notifier.run(); // Blocks in epoll_wait

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
