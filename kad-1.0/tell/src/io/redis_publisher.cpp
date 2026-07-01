#include "redis_publisher.hpp"
#include <iostream>

namespace kad::io {

    RedisPublisher::RedisPublisher(const std::string& broker_url)
        : broker_url_(broker_url), connected_(false) {
    }

    RedisPublisher::~RedisPublisher() {
        disconnect();
    }

    void RedisPublisher::connect() {
        try {
            redis_ = std::make_unique<sw::redis::Redis>(broker_url_);
            // simple ping to test connection
            redis_->ping();
            connected_ = true;
            std::cout << "RedisPublisher connected to " << broker_url_ << std::endl;
        } catch (const sw::redis::Error& e) {
            std::cerr << "RedisPublisher connect error: " << e.what() << std::endl;
            connected_ = false;
        }
    }

    void RedisPublisher::disconnect() {
        connected_ = false;
        redis_.reset();
    }

    bool RedisPublisher::isConnected() const {
        return connected_;
    }

    void RedisPublisher::publish(const std::string& channel, const std::string& payload) {
        if (connected_ && redis_) {
            try {
                redis_->publish(channel, payload);
            } catch (const sw::redis::Error& e) {
                std::cerr << "RedisPublisher publish error: " << e.what() << std::endl;
            }
        }
    }

    void RedisPublisher::setAttribute(const std::string& key, const std::string& value) {
        if (connected_ && redis_) {
            try {
                redis_->set(key, value);
            } catch (const sw::redis::Error& e) {
                std::cerr << "RedisPublisher setAttribute error: " << e.what() << std::endl;
            }
        }
    }

    std::string RedisPublisher::getAttribute(const std::string& key) {
        if (connected_ && redis_) {
            try {
                auto val = redis_->get(key);
                if (val) return *val;
            } catch (const sw::redis::Error& e) {
                std::cerr << "RedisPublisher getAttribute error: " << e.what() << std::endl;
            }
        }
        return "";
    }

} // namespace kad::io
