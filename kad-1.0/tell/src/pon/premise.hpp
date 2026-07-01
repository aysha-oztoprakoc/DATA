#pragma once
#ifndef KAD_PON_PREMISE_HPP
#define KAD_PON_PREMISE_HPP

#include <string>
#include <functional>

namespace kad::pon {

    enum class PremiseOp {
        CHANGED,
        NOT_EMPTY,
        EQUALS,
        NOT_EQUALS,
        GREATER_THAN
    };

    /**
     * Premise
     * 
     * Represents a condition regarding a SharedAttribute.
     * Evaluated by a Rule when the Notifier triggers it.
     */
    class Premise {
    private:
        std::string target_attribute_;
        PremiseOp operation_;
        std::function<bool()> evaluation_func_;

    public:
        // Generic constructor leveraging a lambda to evaluate the premise
        Premise(const std::string& target_attribute, PremiseOp operation, std::function<bool()> func);

        // Helper template constructors would ideally live here to bind to SharedAttribute<T>
        // But since we want to keep dependencies clean, we use the lambda approach.

        const std::string& targetAttribute() const;
        bool evaluate() const;
    };

} // namespace kad::pon

#endif // KAD_PON_PREMISE_HPP
