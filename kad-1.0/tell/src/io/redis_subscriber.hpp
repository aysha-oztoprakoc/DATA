#pragma once
#ifndef KAD_IO_REDIS_SUBSCRIBER_HPP
#define KAD_IO_REDIS_SUBSCRIBER_HPP

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <sw/redis++/redis++.h>

namespace kad::io {

    class RedisSubscriber {
    private:
        std::unique_ptr<sw::redis::Redis> redis_;
        std::unique_ptr<sw::redis::Subscriber> sub_;
        std::string broker_url_;
        std::atomic<bool> connected_;
        std::unordered_map<std::string, std::function<void(const std::string&)>> handlers_;
        std::thread loop_thread_;

    public:
        RedisSubscriber(const std::string& broker_url);
        ~RedisSubscriber();

        void connect();
        void disconnect();
        bool isConnected() const;

        void subscribe(const std::string& channel, std::function<void(const std::string&)> handler);
    };

} // namespace kad::io

#endif // KAD_IO_REDIS_SUBSCRIBER_HPP
