
#include <Mahi/Daq/NI/MyRio/MyRio.hpp>
#include "Detail/MyRioFpga60/MyRio.h"
#include "Detail/MyRioFpga60/MyRio.h"
#include "MyRioUtils.hpp"
#include <thread>
#include <chrono>
#include <Mahi/Util/Logging/Log.hpp>
using namespace mahi::util;

#define MyRio_DefaultFolder "/var/local/natinst/bitfiles/"
#define MyRio_BitfilePath MyRio_DefaultFolder MyRio_Bitfile

extern NiFpga_Session myrio_session;

namespace mahi {
namespace daq {

namespace {

/// Custom open function adapted from NI version
bool open_myrio(bool reset) {
    NiFpga_Status status;
    NiFpga_Bool sysReady;
    // init
    status = NiFpga_Initialize();
    if (MyRio_IsNotSuccess(status)) {
        LOG(Error) << "Failed to initializae myRIO FPGA " << get_nifpga_error_message(status);
        return false;
    }
    LOG(Verbose) << "Initialized myRIO FPGA";
    // open but don't run yet
    status = NiFpga_Open(MyRio_BitfilePath, MyRio_Signature, "RIO0", NiFpga_OpenAttribute_NoRun, &myrio_session);
    if (MyRio_IsNotSuccess(status)) {
        LOG(Error) << "Failed to open myRIO FPGA " << get_nifpga_error_message(status);
        if (status == NiFpga_Status_BitfileReadError) {
            LOG(Error) << "Ensure NiFpga_MyRio1900Fpga60.lvbitx is in " <<  MyRio_DefaultFolder;
        }
        return false;
    }
    LOG(Verbose) << "Opened myRIO FPGA";
    // reset FPGA if requested
    if (reset) {
        status = NiFpga_Reset(myrio_session);
        if (MyRio_IsNotSuccess(status)) {
            LOG(Error) << "Failed to reset myRIO FGPA " << get_nifpga_error_message(status);
            return false;
        }
        LOG(Verbose) << "Reset myRIO FPGA";
    }
    // run FPGA
    status = NiFpga_Run(myrio_session, 0);
    if (MyRio_IsNotSuccess(status)) {
        if (status == -NiFpga_Status_FpgaAlreadyRunning) {
            LOG(Info) << "myRIO FPGA was already running";
            return true;
        }
        else {
            LOG(Error) << "Failed to run myRIO FPGA " << get_nifpga_error_message(status);
            return false;
        }
    }
    LOG(Verbose) << "Running myRIO FPGA";
    // wait for the FPGA to signal ready
    std::chrono::seconds timeout(5);
    auto begin = std::chrono::high_resolution_clock::now();
    sysReady = NiFpga_False;    
    while (std::chrono::high_resolution_clock::now() - begin < timeout && !MyRio_IsNotSuccess(status) && !sysReady) {
        NiFpga_MergeStatus(&status, NiFpga_ReadBool(myrio_session, SYSRDY, &sysReady));
    }
    if (MyRio_IsNotSuccess(status)) {
        LOG(Error) << "Failed to read SYSRDY register " << get_nifpga_error_message(status);
        return false;
    }
    if (!sysReady) {
        LOG(Error) << "Timeout while waiting for system ready";
        return false;
    }
    LOG(Verbose) << "myRIO FPGA system ready";    
    return true;
}

/// Custom close function adapted from NI version
bool close_myrio(bool reset) {
    NiFpga_Status status;
    // close FPGA
    status = reset ? NiFpga_Close(myrio_session, 0) : NiFpga_Close(myrio_session, NiFpga_CloseAttribute_NoResetIfLastSession);
    if (MyRio_IsNotSuccess(status)) {
        LOG(Error) << "Failed to close myRIO FPGA " << get_nifpga_error_message(status);
        return false;
    }
    LOG(Verbose) << "Closed myRIO FPGA";
    // unload the NiFpga library
    status = NiFpga_Finalize();
    if (MyRio_IsNotSuccess(status)) {
        MyRio_PrintStatus(status);
        LOG(Error) << "Failed to unload myRIO FPGA library " << get_nifpga_error_message(status);
        return false;
    }
    LOG(Verbose) << "Finalized myRIO FPGA";
    return true;
}

}

MyRio::MyRio() :
    Daq("myRIO"),
    mxpA(*this, MyRioConnector::Type::MxpA),
    mxpB(*this, MyRioConnector::Type::MxpB),
    mspC(*this, MyRioConnector::Type::MspC),
    LED(*this, {0,1,2,3})
{
    // open
    open();
    // LED chs
    LED.set_channels({0,1,2,3});
}

MyRio::~MyRio() {
    if (is_enabled())
        disable();
    if (is_open())
        close();
}

void MyRio::print_registers() const {
    ::mahi::daq::print_registers();
}


// bool MyRio::reset() {
//     if (is_open()) {       
//         if (close_myrio(true) && open_myrio(true)) {
//             mxpA.reset();
//             mxpB.reset();
//             mspC.reset();
//             LOG(Info) << "Reset myRIO FPGA to default state";
//         }
//         else {
//             LOG(Info) << "Failed to reset myRIO FPGA to default state";
//         }
//         return true;
//     }
//     else {
//         LOG(Error) << "Could not reset myRIO FPGA because the myRIO is not open";
//         return false;
//     }    
// }

bool MyRio::on_daq_open() {   
    return (open_myrio(false) &&  mxpA.open() &&  mxpB.open() && mspC.open());
}

bool MyRio::on_daq_close() {
    return (close_myrio(false) && mxpA.close() && mxpB.close() && mspC.close());
}

bool MyRio::on_daq_enable() {
    // enable each connector
    if (mxpA.enable() && mxpB.enable() && mspC.enable()) {
        util::sleep(milliseconds(10));
        return true;
    }
    else
        return false;
}

bool MyRio::on_daq_disable() {
    // disable each connect
    if (mxpA.disable() && mxpB.disable() && mspC.disable()) {
        util::sleep(milliseconds(10));
        return true;
    }
    else
        return false;
}

bool MyRio::read_all() {
    bool readSelf = Daq::read_all();
    bool readA = mxpA.read_all();
    bool readB = mxpB.read_all();
    bool readC = mspC.read_all();
    return readSelf && readA && readB && readC;
}

bool MyRio::write_all() {
    bool writeSelf = Daq::write_all();
    bool writeA = mxpA.write_all();
    bool writeB = mxpB.write_all();
    bool writeC = mspC.write_all();
    return writeSelf && writeA && writeB && writeC;
}

bool MyRio::is_button_pressed() const {
    return get_bit(DIBTN, 0);
}

} // namespace daq
} // namespace mahi
