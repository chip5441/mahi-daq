#include <Mahi/Daq/Quanser/QuanserOther.hpp>
#include <Mahi/Daq/Quanser/QuanserDaq.hpp>
#include <Mahi/Util/Logging/Log.hpp>
#include <hil.h>
#include "QuanserUtils.hpp"

using namespace mahi::util;

namespace mahi {
namespace daq {

QuanserOtherInput::QuanserOtherInput(QuanserDaq& d, QuanserHandle& h, const ChanNums& allowed) : 
    ChanneledModule(d,allowed),
    ReadBuffer<double>(*this, 0), 
    m_h(h)
{ 
    set_name(d.name() + ".OI");
    auto on_read_impl = [this](const ChanNum* chs, double* vals, std::size_t n) {
        t_error result = hil_read_other(m_h, chs, static_cast<t_uint32>(n), vals);
        if (result == 0)
            return true;
        else {
            LOG(Error) << "Failed to read " << name() << " " << quanser_msg(result);
            return false;
        }
        return true;
    };
    connect_read(*this, on_read_impl);
    read_with_all = true;
}


QuanserOtherOutput::QuanserOtherOutput(QuanserDaq& d, QuanserHandle& h, const ChanNums& allowed) : 
    ChanneledModule(d,allowed),
    WriteBuffer<double>(*this, 0), 
    m_h(h)
{ 
    set_name(d.name() + ".OO");
    auto on_read_impl = [this](const ChanNum* chs, const double* vals, std::size_t n) {
        t_error result = hil_write_other(m_h, chs, static_cast<t_uint32>(n), vals);
        if (result == 0)
            return true;
        else {
            LOG(Error) << "Failed to write " << name() << " " << quanser_msg(result);
            return false;
        }
        return true;
    };
    connect_write(*this, on_read_impl);
    write_with_all = true;
}

} // namespace daq
} // namespace mahi