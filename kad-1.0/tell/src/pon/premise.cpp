#include "premise.hpp"

namespace kad::pon {

    Premise::Premise(const std::string& target_attribute, PremiseOp operation, std::function<bool()> func)
        : target_attribute_(target_attribute), operation_(operation), evaluation_func_(std::move(func)) {}

    const std::string& Premise::targetAttribute() const {
        return target_attribute_;
    }

    bool Premise::evaluate() const {
        if (evaluation_func_) {
            return evaluation_func_();
        }
        return false;
    }

} // namespace kad::pon
