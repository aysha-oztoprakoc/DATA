#pragma once
#ifndef KAD_FBE_BASE_HPP
#define KAD_FBE_BASE_HPP

#include <string>

namespace kad::pon {
    class Notifier;
}

namespace kad::fbe {

    /**
     * FBE_Base
     * 
     * Base class for all Fact Base Elements.
     * In PON, FBEs represent physical or logical entities that possess
     * state (Attributes) and operations (Methods), but absolutely NO 
     * conditional logic.
     */
    class FBE_Base {
    protected:
        std::string name_;
        kad::pon::Notifier* notifier_;

    public:
        FBE_Base(const std::string& name, kad::pon::Notifier* notifier);
        virtual ~FBE_Base() = default;

        const std::string& name() const;
    };

} // namespace kad::fbe

#endif // KAD_FBE_BASE_HPP
