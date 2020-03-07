#include <hil.h>
#include <Mahi/Daq/Quanser/QuanserAO.hpp>
#include <Mahi/Daq/Quanser/QuanserDaq.hpp>

#include <Mahi/Util/Logging/Log.hpp>
using namespace mahi::util;

namespace mahi {
namespace daq {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

QuanserAO::QuanserAO(QuanserDaq& daq, const ChanNums& channel_numbers) :
    AnalogOutput(channel_numbers),
    daq_(daq)
{
    set_name(daq.get_name() + "_AO");
}

bool QuanserAO::update() {
    t_error result;
    result = hil_write_analog(daq_.handle_, &channel_numbers()[0], static_cast<ChanNum>(channel_count()), &m_values.get()[0]);
    if (result == 0)
        return true;
    else {
        LOG(Error) << "Failed to update " << get_name() << " " << QuanserDaq::get_quanser_error_message(result);
        return false;
    }
}

bool QuanserAO::update_channel(ChanNum channel_number) {
    t_error result;
    result = hil_write_analog(daq_.handle_, &channel_number, 1, &m_values[channel_number]);
    if (result == 0)
        return true;
    else {
        LOG(Error) << "Failed to update " << get_name() << " channel number " << channel_number << " " << QuanserDaq::get_quanser_error_message(result);
        return false;
    }
}

bool QuanserAO::set_ranges(const std::vector<Voltage>& min_values, const std::vector<Voltage>& max_values) {
    if (!Module::set_ranges(min_values, max_values))
        return false;
    t_error result;
    result = hil_set_analog_output_ranges(daq_.handle_, &channel_numbers()[0], static_cast<ChanNum>(channel_count()), &min_values_.get()[0], &max_values_.get()[0]);
    if (result == 0) {
        LOG(Verbose) << "Set " << get_name() << " ranges"; // to min=" << min_values << ", max=" << max_values;
        return true;
    }
    else {
        LOG(Error) << "Failed to set " << get_name() << " ranges " << QuanserDaq::get_quanser_error_message(result);
        return false;
    }
}

bool QuanserAO::set_range(ChanNum channel_number, Voltage min_value, Voltage max_value) {
    if (!Module::set_range(channel_number, min_value, max_value))
        return false;
    t_error result;
    result = hil_set_analog_output_ranges(daq_.handle_, &channel_number, 1, &min_values_[channel_number], &max_values_[channel_number]);
    if (result == 0) {
        LOG(Verbose) << "Set " << get_name() << " channel number " << channel_number << " range to min=" << min_value << ", max=" << max_value;
        return true;
    }
    else {
        LOG(Error) << "Failed to set " << get_name() << " channel number " << channel_number << " range " << QuanserDaq::get_quanser_error_message(result);
        return false;
    }
}

bool QuanserAO::set_expire_values(const std::vector<Voltage>& expire_values) {
    if (!Output::set_expire_values(expire_values))
        return false;
    t_error result;
    result = hil_watchdog_set_analog_expiration_state(daq_.handle_, &channel_numbers()[0], static_cast<ChanNum>(channel_count()), &expire_values_.get()[0]);
    if (result == 0) {
        LOG(Verbose) << "Set " << get_name() << " expire values to " << expire_values_;
        return true;
    }
    else {
        LOG(Error) << "Failed to set " << get_name() << " expire values " << QuanserDaq::get_quanser_error_message(result);
        return false;
    }
}

bool QuanserAO::set_expire_value(ChanNum channel_number, Voltage expire_value) {
    if (!Output::set_expire_value(channel_number, expire_value))
        return false;
    t_error result;
    result = hil_watchdog_set_analog_expiration_state(daq_.handle_, &channel_number, 1, &expire_values_[channel_number]);
    if (result == 0) {
        LOG(Verbose) << "Set " << get_name() << " channel number " << channel_number << " expire value to " << expire_value;
        return true;
    }
    else {
        LOG(Error) << "Failed to set " << get_name() << " channel number " << channel_number << " expire value " << QuanserDaq::get_quanser_error_message(result);
        return false;
    }
}

} // namespace daq
} // namespace mahi
