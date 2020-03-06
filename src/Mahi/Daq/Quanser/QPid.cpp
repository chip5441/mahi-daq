#include <mahi/daq/Quanser/QPid.hpp>
#include <hil.h>
#include <thread>

#include <Mahi/Util/Logging/Log.hpp>
using namespace mahi::util;

namespace mahi {
namespace daq {

//==============================================================================
// STATIC VARIABLES
//==============================================================================

 ChanNum NEXT_QPID_ID = 0;

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

QPid::QPid(QuanserOptions options,
             ChanNum id) :
    QuanserDaq("qpid_e", id, options),
    AI(*this, { 0, 1, 2, 3, 4, 5, 6, 7 }),
    AO(*this, { 0, 1, 2, 3, 4, 5, 6, 7 }),
    DIO(*this, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 
                 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
                 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
                 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
                 50, 51, 52, 53, 54, 55  }),
    encoder(*this, { 0, 1, 2, 3, 4, 5, 6, 7 }, true), // has velocity estimation
    watchdog(*this, 100_ms)
{
    // increment NEXT_ID
    ++NEXT_QPID_ID;
}

QPid::~QPid() {
    // if enabled, disable
    if (is_enabled())
        disable();
    // if open, close
    if (is_open()) {
        // set default options on program end
        set_options(QuanserOptions());
        close();
    }
    // decrement NEXT_ID
    --NEXT_QPID_ID;
}

bool QPid::on_open() {
    // open as QDaq
    if (!QuanserDaq::on_open())
        return false;
    // clear watchdog (precautionary, ok if fails)
    watchdog.stop();
    // clear the watchdog (precautionary, ok if fails)
    watchdog.clear();
    // set default expire values (digital = LOW, analog = 0.0V)
    if (!AO.set_expire_values(std::vector<Voltage>(8, 0.0))) {
        close();
        return false;
    }
    if (!DIO.set_expire_values(std::vector<Logic>(56, Low))) {
        close();
        return false;
    }

    // gotta do this until PWM class implemented so watchdog works
    ChanNums pwm_channels = {0,1,2,3,4,5,6,7};
    std::vector<double> pwm_exp_vals(8, 0.0);
    t_error result = hil_watchdog_set_pwm_expiration_state(handle_, &pwm_channels[0], 8, &pwm_exp_vals[0]);
    if (result != 0)
        LOG(Error) << "Failed to set PWM expiration states on QPID " << get_name() << " " << QuanserDaq::get_quanser_error_message(result);

    // allow changes to take effect
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    return true;
}

bool QPid::on_close() {
    // stop watchdog (precautionary, ok if fails)
    watchdog.stop();
    // clear the watchdog (precautionary, ok if fails)
    watchdog.clear();
    // allow changes to take effect
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    // close as QDaq
    return QuanserDaq::on_close();
}

bool QPid::on_enable() {
    if (!is_open()) {
        LOG(Error) << "Unable to enable Q8-USB " << get_name() << " because it is not open";
        return false;
    }
    bool success = true;
    // enable each module
    if (!AI.enable())
        success = false;
    if (!AO.enable())
        success = false;
    if (!DIO.enable())
        success = false;
    if (!encoder.enable())
        success = false;
    // allow changes to take effect
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    return success;
}

bool QPid::on_disable() {
    if (!is_open()) {
        LOG(Error) << "Unable to disable Q8-USB " << get_name() << " because it is not open";
        return false;
    }
    bool success = true;
    // disable each module
    if (!AI.disable())
        success = false;
    if (!AO.disable())
        success = false;
    if (!DIO.disable())
        success = false;
    if (!encoder.disable())
        success = false;
    // allow changes to take effect
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    return success;
}

bool QPid::update_input() {
    if (!is_enabled()) {
        LOG(Error) << "Unable to update " << get_name() << " input because it is disabled";
        return false;
    }
    t_error result;
    result = hil_read(handle_,
        AI.channel_count() > 0 ? &(AI.channel_numbers())[0] : NULL,
        static_cast<ChanNum>(AI.channel_count()),
        encoder.channel_count() > 0 ? &(encoder.channel_numbers())[0] : NULL,
        static_cast<ChanNum>(encoder.channel_count()),
        DIO.input_channel_count() > 0 ? &(DIO.input_channel_numbers())[0] : NULL,
        static_cast<ChanNum>(DIO.input_channel_count()),
        encoder.channel_count() > 0 ? &(encoder.get_quanser_velocity_channels())[0] : NULL,
        static_cast<ChanNum>(encoder.channel_count()),
        AI.channel_count() > 0 ? &(AI.get())[0] : NULL,
        encoder.channel_count() > 0 ? &(encoder.get())[0] : NULL,
        DIO.input_channel_count() > 0 ? &(DIO.quanser_input_buffer_)[0] : NULL,
        encoder.channel_count() > 0 ? &(encoder.get_values_per_sec())[0] : NULL);   
    DIO.buffer_to_input();
    if (result == 0)
        return true;
    else {
        LOG(Error) << "Failed to update " << get_name() << " input " << QuanserDaq::get_quanser_error_message(result);
        return false;
    }
}

bool QPid::update_output() {
    if (!is_enabled()) {
        LOG(Error) << "Unable to update " << get_name() << " output because it is disabled";
        return false;
    }
    // convert digitals
    DIO.output_to_buffer();
    t_error result;
    result = hil_write(handle_,
        AO.channel_count() > 0 ? &(AO.channel_numbers())[0] : NULL,
        static_cast<ChanNum>(AO.channel_count()),
        NULL, 0,
        DIO.output_channel_count() > 0 ? &(DIO.output_channel_numbers())[0] : NULL,
        static_cast<ChanNum>(DIO.output_channel_count()),
        NULL, 0,
        AO.channel_count() > 0 ? &(AO.get())[0] : NULL,
        NULL,
        DIO.output_channel_count() > 0 ? &(DIO.quanser_output_buffer_)[0] : NULL,
        NULL);
    if (result == 0)
        return true;
    else {
        LOG(Error) << "Failed to update " << get_name() << " output " << QuanserDaq::get_quanser_error_message(result);
        return false;
    }
}

ChanNum QPid::next_id() {
    return NEXT_QPID_ID;
}

} // namespace daq
} // namespace mahi

