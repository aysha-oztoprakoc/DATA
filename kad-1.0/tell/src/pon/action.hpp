#pragma once
#ifndef KAD_PON_ACTION_HPP
#define KAD_PON_ACTION_HPP

#include <string>
#include <functional>

namespace kad::pon {

    /**
     * Action
     * 
     * A callable executed by a Rule when all its Premises are satisfied.
     * Often invokes a Method of an FBE or publishes to MQTT.
     */
    class Action {
    protected:
        std::string name_;
        std::function<void()> execute_func_;

    public:
        Action(const std::string& name, std::function<void()> func);
        virtual ~Action() = default;

        const std::string& name() const;
        virtual void execute() const;
    };

} // namespace kad::pon

#endif // KAD_PON_ACTION_HPP
