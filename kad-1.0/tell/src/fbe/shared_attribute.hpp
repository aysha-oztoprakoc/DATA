#pragma once
#ifndef KAD_FBE_SHARED_ATTRIBUTE_HPP
#define KAD_FBE_SHARED_ATTRIBUTE_HPP

#include <string>
#include <mutex>

#include "../pon/notifier.hpp"

namespace kad::fbe {

    /**
     * SharedAttribute<T>
     * 
     * A fundamental component of the Notification-Oriented Paradigm (PON).
     * Attributes hold state. When mutated via set(), they compare with their
     * previous state. If altered, they notify the central Notifier, which
     * in turn evaluates any Premises observing this attribute.
     * 
     * Zero-polling, completely reactive.
     */
    template<typename T>
    class SharedAttribute {
    private:
        std::string name_;
        T value_;
        T previous_value_;
        bool changed_ = false;
        kad::pon::Notifier* notifier_ = nullptr;
        mutable std::mutex mutex_;

    public:
        SharedAttribute(const std::string& name, const T& initial, kad::pon::Notifier* notifier)
            : name_(name), value_(initial), previous_value_(initial), notifier_(notifier) {}

        void set(const T& new_value) {
            {
                std::lock_guard<std::mutex> lock(mutex_);
                if (value_ == new_value) {
                    return; // No state change, no notification
                }
                previous_value_ = value_;
                value_ = new_value;
                changed_ = true;
            }
            
            // Trigger the logico-causal layer evaluation outside the lock
            if (notifier_) {
                notifier_->onAttributeChanged(name_);
            }
        }

        T get() const {
            std::lock_guard<std::mutex> lock(mutex_);
            return value_;
        }

        T previous() const {
            std::lock_guard<std::mutex> lock(mutex_);
            return previous_value_;
        }

        bool changed() const {
            std::lock_guard<std::mutex> lock(mutex_);
            return changed_;
        }

        const std::string& name() const {
            return name_;
        }

        void resetChanged() {
            std::lock_guard<std::mutex> lock(mutex_);
            changed_ = false;
        }
    };

} // namespace kad::fbe

#endif // KAD_FBE_SHARED_ATTRIBUTE_HPP
