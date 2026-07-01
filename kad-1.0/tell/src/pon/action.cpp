#include "action.hpp"

namespace kad::pon {

    Action::Action(const std::string& name, std::function<void()> func)
        : name_(name), execute_func_(std::move(func)) {}

    const std::string& Action::name() const {
        return name_;
    }

    void Action::execute() const {
        if (execute_func_) {
            execute_func_();
        }
    }

} // namespace kad::pon
