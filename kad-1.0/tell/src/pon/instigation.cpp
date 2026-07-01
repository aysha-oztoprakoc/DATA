#include "instigation.hpp"

namespace kad::pon {

    Instigation::Instigation(const std::string& name, const std::string& target_attribute, std::function<void()> func)
        : Action(name, std::move(func)), target_attribute_(target_attribute) {}

    const std::string& Instigation::targetAttribute() const {
        return target_attribute_;
    }

} // namespace kad::pon
