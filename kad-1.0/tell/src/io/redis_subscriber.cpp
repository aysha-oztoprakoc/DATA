#include "redis_subscriber.hpp"
#include <iostream>
#include <chrono>

namespace kad::io {

    RedisSubscriber::RedisSubscriber(const std::string& broker_url)
        : broker_url_(broker_url), connected_(false) {}

    RedisSubscriber::~RedisSubscriber() {
        disconnect();
    }

    void RedisSubscriber::connect() {
        try {
            redis_ = std::make_unique<sw::redis::Redis>(broker_url_);
            sub_ = std::make_unique<sw::redis::Subscriber>(redis_->subscriber());
            connected_ = true;

            sub_->on_message([this](std::string channel, std::string msg) {
                if (handlers_.count(channel)) {
                    handlers_[channel](msg);
                }
            });

            loop_thread_ = std::thread([this]() {
                while (connected_) {
                    try {
                        sub_->consume();
                    } catch (const sw::redis::TimeoutError& e) {
                        continue;
                    } catch (const sw::redis::Error& e) {
                        std::cerr << "RedisSubscriber consume error: " << e.what() << std::endl;
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                    }
                }
            });

            std::cout << "RedisSubscriber connected to " << broker_url_ << std::endl;
        } catch (const sw::redis::Error& e) {
            std::cerr << "RedisSubscriber connect error: " << e.what() << std::endl;
            connected_ = false;
        }
    }

    void RedisSubscriber::disconnect() {
        connected_ = false;
        if (loop_thread_.joinable()) {
            loop_thread_.join();
        }
        sub_.reset();
        redis_.reset();
    }

    bool RedisSubscriber::isConnected() const {
        return connected_;
    }

    void RedisSubscriber::subscribe(const std::string& channel, std::function<void(const std::string&)> handler) {
        handlers_[channel] = std::move(handler);
        if (connected_ && sub_) {
            sub_->subscribe(channel);
        }
    }

} // namespace kad::io
