#include <hil.h>
#include <mahi/daq/Quanser/QuanserDaq.hpp>
#include <mahi/daq/Quanser/QuanserPwm.hpp>

#if MAHI_DAQ_OUTPUT_LOGS
    #ifdef MAHI_LOG
        #include <mahi/log/Log.hpp>
    #else
        #include <iostream>
        #define LOG(severity) std::cout << std::endl << #severity << ": "
    #endif
#else
    #include <iostream>
    #define LOG(severity) if (true) { } else std::cout 
#endif

namespace mahi {
namespace daq {

QuanserPwm::QuanserPwm(QuanserDaq& daq, const ChanNums& channel_numbers) :
    PwmOutput(channel_numbers),
    daq_(daq)
{
    set_name(daq.get_name() + "_PWM");
}

bool QuanserPwm::update() {
    t_error result;
    result = hil_write_pwm(daq_.handle_, &get_channel_numbers()[0], static_cast<ChanNum>(get_channel_count()), &values_.get()[0]);
    if (result == 0)
        return true;
    else {
        LOG(Error) << "Failed to update " << get_name() << " " << QuanserDaq::get_quanser_error_message(result);
        return false;
    }
}

bool QuanserPwm::update_channel(ChanNum channel_number) {
    t_error result;
    result = hil_write_pwm(daq_.handle_, &channel_number, 1, &values_[channel_number]);
    if (result == 0)
        return true;
    else {
        LOG(Error) << "Failed to update " << get_name() << " channel number " << channel_number << " " << QuanserDaq::get_quanser_error_message(result);
        return false;
    }
}

bool QuanserPwm::set_expire_values(const std::vector<DutyCycle>& expire_values) {
    if (!Output::set_expire_values(expire_values))
        return false;
    t_error result;
    result = hil_watchdog_set_pwm_expiration_state(daq_.handle_, &get_channel_numbers()[0], static_cast<ChanNum>(get_channel_count()), &expire_values_.get()[0]);
    if (result == 0) {
        LOG(Verbose) << "Set " << get_name() << " expire values to " << expire_values_;
        return true;
    }
    else {
        LOG(Error) << "Failed to set " << get_name() << " expire values " << QuanserDaq::get_quanser_error_message(result);
        return false;
    }
}

bool QuanserPwm::set_expire_value(ChanNum channel_number, DutyCycle expire_value) {
    if (!Output::set_expire_value(channel_number, expire_value))
        return false;
    t_error result;
    result = hil_watchdog_set_pwm_expiration_state(daq_.handle_, &channel_number, 1, &expire_values_[channel_number]);
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