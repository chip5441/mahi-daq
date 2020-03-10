#include <Mahi/Daq/Quanser2/QuanserDO.hpp>
#include <Mahi/Daq/Quanser2/QuanserDaq.hpp>
#include <hil.h>
#include "QuanserUtils.hpp"
#include <Mahi/Util/Logging/Log.hpp>

using namespace mahi::util;

namespace mahi {
namespace daq {

QuanserDO::QuanserDO(QuanserDaq& d, QuanserHandle& h, const ChanNums& channel_numbers)  : 
    Fused<DOModule,QuanserDaq>(d),
    expire_values(*this, 0), m_h(h)
{
    set_name(d.name() + ".DO");
    set_channel_numbers(channel_numbers);
    /// Write Channels
    auto on_write_impl = [this](const ChanNum *chs, const Logic *vals, std::size_t n) {
        t_error result = hil_write_digital(m_h, chs, static_cast<t_uint32>(n), vals);
        if (result != 0) {
            LOG(Error) << "Failed to write " << this->name() << " digital outputs " << get_quanser_error_message(result);
            return false;
        }
        return true;
    };
    on_write.connect(on_write_impl);
    // // Write Expire States
    auto expire_write_impl = [this](const ChanNum* chs, const Logic* vals, std::size_t n) { 
        // convert to Quanser t_digital_state
        std::vector<t_digital_state> converted(n);
        for (int i = 0; i < n; ++i) {
            if (vals[i] == HIGH)
                converted.push_back(DIGITAL_STATE_HIGH);
            else
                converted.push_back(DIGITAL_STATE_LOW);
        }
        t_error result;
        result = hil_watchdog_set_digital_expiration_state(m_h, chs, static_cast<ChanNum>(n), &converted[0]);
        if (result == 0) {
            LOG(Verbose) << "Set " << name() << " digital output expiration sates";
            return true;
        }
        else {
            LOG(Error) << "Failed to set " << name() << " digital output expire states " << get_quanser_error_message(result);
            return false;
        }
    };
    expire_values.on_write.connect(expire_write_impl);
}

bool QuanserDO::on_daq_open() {
    for (auto& ch : channels())
        expire_values[ch] = LOW;
    return expire_values.write(); // may need a sleep here
}

} // namespace daq 
} // namespace mahi