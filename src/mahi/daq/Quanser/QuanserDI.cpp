#include <mahi/daq/Quanser/QuanserDaq.hpp>
#include <mahi/daq/Quanser/QuanserDI.hpp>
#include <hil.h>

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

    //==============================================================================
    // CLASS DEFINITIONS
    //==============================================================================

    QuanserDI::QuanserDI(QuanserDaq& daq, const ChanNums& channel_numbers) :
        DigitalInput(channel_numbers),
        daq_(daq),
        quanser_values_(this)
    {
        set_name(daq_.get_name() + "_DI");
    }

    bool QuanserDI::update() {
        t_error result;
        result = hil_read_digital(daq_.handle_, &get_channel_numbers()[0], static_cast<ChanNum>(get_channel_count()), &quanser_values_.get()[0]);
        if (result == 0) {
            // convert Quanser t_boolean (aka char) to Logic
            for (auto const& ch : get_channel_numbers())
                values_[ch] = static_cast<Logic>(quanser_values_[ch]);
            return true;
        }
        else {
            LOG(Error) << "Failed to update " << get_name() << " " << QuanserDaq::get_quanser_error_message(result);
            return false;
        }
    }

    bool QuanserDI::update_channel(ChanNum channel_number) {
        t_error result;
        result = hil_read_digital(daq_.handle_, &channel_number, 1, &quanser_values_[channel_number]);
        if (result == 0) {
            values_[channel_number] = static_cast<Logic>(quanser_values_[channel_number]);
            return true;
        }
        else {
            LOG(Error) << "Failed to update " << get_name() << " channel number " << channel_number << " "  << QuanserDaq::get_quanser_error_message(result);
            return false;
        }
    }

    std::vector<char>& QuanserDI::get_quanser_values() {
        return quanser_values_.get();
    }




} // namespace daq
} // namespace mahi
