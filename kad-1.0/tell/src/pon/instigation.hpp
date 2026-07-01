#pragma once
#ifndef KAD_PON_INSTIGATION_HPP
#define KAD_PON_INSTIGATION_HPP

#include "action.hpp"
#include <string>

namespace kad::pon {

    /**
     * Instigation
     * 
     * Semantically distinct from a generic Action. 
     * An Instigation is explicitly responsible for mutating another
     * SharedAttribute via its set() method, thereby propagating 
     * the notification chain.
     */
    class Instigation : public Action {
    private:
        std::string target_attribute_;

    public:
        Instigation(const std::string& name, const std::string& target_attribute, std::function<void()> func);

        const std::string& targetAttribute() const;
    };

} // namespace kad::pon

#endif // KAD_PON_INSTIGATION_HPP
