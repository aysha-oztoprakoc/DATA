#pragma once
#ifndef KAD_PON_RULE_HPP
#define KAD_PON_RULE_HPP

#include "premise.hpp"
#include "action.hpp"
#include <vector>
#include <string>
#include <memory>

namespace kad::pon {

    /**
     * Rule
     * 
     * A logical unit in the PON paradigm: {Premises} -> {Actions}.
     */
    class Rule {
    private:
        std::string name_;
        std::vector<Premise> premises_;
        std::vector<std::shared_ptr<Action>> actions_; // shared_ptr to support polymorphism (Instigation)

    public:
        Rule(const std::string& name, std::vector<Premise> premises, std::vector<std::shared_ptr<Action>> actions);

        const std::string& name() const;
        const std::vector<Premise>& premises() const;

        bool evaluate() const;
        void execute() const;
        bool isReady() const; // Equivalent to evaluate()
    };

} // namespace kad::pon

#endif // KAD_PON_RULE_HPP
