#include <Mahi/Daq/Quanser2/QuanserEncoder.hpp>
#include <Mahi/Daq/Quanser2/QuanserAO.hpp>
#include <Mahi/Daq/Quanser2/QuanserDaq.hpp>
#include <hil.h>
#include "QuanserUtils.hpp"
#include <Mahi/Util/Logging/Log.hpp>
#include <Mahi/Util/Print.hpp>

using namespace mahi::util;

namespace mahi {
namespace daq {

QuanserEncoder::QuanserEncoder(QuanserDaq& d, QuanserHandle& h, const ChanNums& allowed) : 
    Fused<EncoderModule<QuanserEncoder>,QuanserDaq>(d,allowed),
    m_h(h)
{
    set_name(d.name() + ".encoder");
    // Read Encoders
    auto read_impl = [this](const ChanNum* chs, int* counts, std::size_t n) {
        t_error result = hil_read_encoder(m_h, chs, static_cast<t_uint32>(n), counts);
        if (result == 0)
            return true;
        else {
            LOG(Error) << "Failed to read " << name() << " " << quanser_msg(result);
            return false;
        }
    };
    on_read.connect(read_impl);
    // Write Encoders
    auto write_impl = [this](const ChanNum* chs, const int* counts, std::size_t n) {
        t_error result = hil_set_encoder_counts(m_h, chs, static_cast<t_uint32>(n), counts);
        util::sleep(milliseconds(10));
        if (result == 0) {
            LOG(Verbose) << "Wrote " << name() << " encoder counts.";
            return true;
        }
        else {
            LOG(Error) << "Failed to write " << name() << " encoder counts " << quanser_msg(result);
            return false;
        }
    };
    on_write.connect(write_impl);
    /// Write Quadratue Factors
    auto write_quad_impl = [this](const ChanNum* chs, const QuadMode* quads, std::size_t n) {
        std::vector<t_encoder_quadrature_mode> converted_factors(n);
        for (int i = 0; i < n; ++i) {
            if (quads[i] == QuadMode::X0)
                converted_factors[i] = ENCODER_QUADRATURE_NONE;
            else if (quads[i] == QuadMode::X1)
                converted_factors[i] = ENCODER_QUADRATURE_1X;
            else if (quads[i] == QuadMode::X2)
                converted_factors[i] = ENCODER_QUADRATURE_2X;
            else if (quads[i] == QuadMode::X4)
                converted_factors[i] = ENCODER_QUADRATURE_4X;
            else {
                LOG(Error) << "QuadMode X" << static_cast<int>(quads[i]) << " not supported by Quanser.";
                return false;
            }
        }
        t_error result = hil_set_encoder_quadrature_mode(m_h, chs, static_cast<t_uint32>(n), &converted_factors[0]);
        util::sleep(milliseconds(10));
        if (result == 0) {
            LOG(Verbose) << "Wrote " << name() << " quadrature factors.";
            return true;
        }
        else {
            LOG(Error) << "Failed to write " << name() << " quadrature factors " << quanser_msg(result);
            return false;
        }
    };
    quadratures.on_write.connect(write_quad_impl);
    /// on channels gained
    auto on_gain = [this](const ChanNums& gained) {
        return quadratures.write(gained, std::vector<QuadMode>(gained.size(), QuadMode::X4));
    };
    on_gain_channels.connect(on_gain);
}

namespace {
    /// Quanser velocity channel indexing offset
    constexpr std::size_t g_v_off = 14000; 
}

QuanserEncoderVelocity::QuanserEncoderVelocity(QuanserDaq& d, QuanserHandle& h, QuanserEncoder& e, const ChanNums& allowed) :
    QuanserOtherInput(d, h, allowed),
    converted(*this, 0),
    m_e(e)
{  
    set_name(d.name() + ".velocity");
    auto convert = [this](const ChanNum * chs, const double* cps, std::size_t n) {
        for (int i = 0; i < n; ++i) {
            /// public facing channel
            ChanNum pch = chs[i] - g_v_off;
            converted.buffer(pch) = static_cast<double>(cps[i]) * m_e.units[pch] / static_cast<double>(m_e.quadratures[pch]);
        }
    };
    post_read.connect(convert);
}

ChanNum QuanserEncoderVelocity::transform_channel_number(ChanNum public_facing) const {
    return public_facing + g_v_off;
}

} // namespace daq 
} // namespace mahi