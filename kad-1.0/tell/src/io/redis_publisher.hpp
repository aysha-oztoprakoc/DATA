#pragma once
#ifndef KAD_IO_REDIS_PUBLISHER_HPP
#define KAD_IO_REDIS_PUBLISHER_HPP

#include <string>
#include <memory>
#include <sw/redis++/redis++.h>

namespace kad::io {

    /**
     * RedisPublisher
     * 
     * Handles Pub/Sub and Attribute setting for the PON architecture.
     */
    class RedisPublisher {
    private:
        std::unique_ptr<sw::redis::Redis> redis_;
        std::string broker_url_;
        bool connected_;

    public:
        RedisPublisher(const std::string& broker_url);
        ~RedisPublisher();

        void connect();
        void disconnect();
        bool isConnected() const;

        void publish(const std::string& channel, const std::string& payload);
        
        // PON Attributes
        void setAttribute(const std::string& key, const std::string& value);
        std::string getAttribute(const std::string& key);
    };

} // namespace kad::io

#endif // KAD_IO_REDIS_PUBLISHER_HPP
