#include "fbe_base.hpp"

namespace kad::fbe {

    FBE_Base::FBE_Base(const std::string& name, kad::pon::Notifier* notifier)
        : name_(name), notifier_(notifier) {}

    const std::string& FBE_Base::name() const {
        return name_;
    }

} // namespace kad::fbe
