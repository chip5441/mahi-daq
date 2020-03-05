#include <hil.h>
#include <mahi/daq/Quanser/QuanserDaq.hpp>
#include <mahi/daq/Quanser/QuanserWatchdog.hpp>

#include <Mahi/Util/Logging/Log.hpp>
using namespace mahi::util;

namespace mahi {
namespace daq {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

QuanserWatchdog::QuanserWatchdog(QuanserDaq& daq, util::Time timeout) :
    Watchdog(timeout),
    daq_(daq)
{ }

QuanserWatchdog::~QuanserWatchdog() {

}

bool QuanserWatchdog::start() {
    t_error result;
    result = hil_watchdog_start(daq_.handle_, timeout_.as_seconds());
    if (result == 0) {
        LOG(Verbose) << "Started watchdog on " << daq_.get_name();
        return true;
    }
    else {
        LOG(Error) << "Failed to start watchdog on " << daq_.get_name() << " " << QuanserDaq::get_quanser_error_message(result);

        return false;
    }
}

bool QuanserWatchdog::kick() {
    t_error result;
    result = hil_watchdog_reload(daq_.handle_);
    if (result == 1) {
        return true;
    }
    else if (result == 0) {
        LOG(Warning) << "Watchdog on " << daq_.get_name() << " expired";
        watching_ = false;
        return false;
    }
    else {
        LOG(Error) << "Failed to kick watchdog on " << daq_.get_name() << " "  << QuanserDaq::get_quanser_error_message(result);
        return false;
    }
}

bool QuanserWatchdog::stop() {
    t_error result;
    result = hil_watchdog_stop(daq_.handle_);
    if (result == 0) {
        LOG(Verbose) << "Stopped watchdog on " << daq_.get_name();
        watching_ = false;
        return true;
    }
    else {
        LOG(Error) << "Failed to stop watchdog on " << daq_.get_name() << " "  << QuanserDaq::get_quanser_error_message(result);
        return false;
    }
}

bool QuanserWatchdog::is_expired() {
    t_error result;
    result = hil_watchdog_is_expired(daq_.handle_);
    if (result == 1) {
        watching_ = false;
        return true;
    }
    else if (result == 0) {
        return false;
    }
    else {
        LOG(Error) << "Failed to check expiration of watchdog on " << daq_.get_name() << " " << QuanserDaq::get_quanser_error_message(result);
        return false;
    }
}

bool QuanserWatchdog::clear() {
    t_error result;
    result = hil_watchdog_clear(daq_.handle_);
    if (result == 0) {
        LOG(Verbose) << "Cleared watchdog on " << daq_.get_name();
        return true;
    }
    else {
        LOG(Error) << "Failed to clear watchdog on " << daq_.get_name() << " " << QuanserDaq::get_quanser_error_message(result);
        return false;
    }
}

} // namespace daq
} // namespace mahi
