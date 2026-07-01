#include "rule.hpp"

namespace kad::pon {

    Rule::Rule(const std::string& name, std::vector<Premise> premises, std::vector<std::shared_ptr<Action>> actions)
        : name_(name), premises_(std::move(premises)), actions_(std::move(actions)) {}

    const std::string& Rule::name() const {
        return name_;
    }

    const std::vector<Premise>& Rule::premises() const {
        return premises_;
    }

    bool Rule::evaluate() const {
        if (premises_.empty()) return false;
        
        for (const auto& premise : premises_) {
            if (!premise.evaluate()) {
                return false;
            }
        }
        return true;
    }

    bool Rule::isReady() const {
        return evaluate();
    }

    void Rule::execute() const {
        for (const auto& action : actions_) {
            if (action) {
                action->execute();
            }
        }
    }

} // namespace kad::pon
