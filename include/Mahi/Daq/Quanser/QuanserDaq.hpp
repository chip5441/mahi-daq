// MIT License
//
// Copyright (c) 2020 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Evan Pezent (epezent@rice.edu)

#pragma once
#include <Mahi/Daq/DaqBase.hpp>
#include <Mahi/Daq/Quanser/QuanserOptions.hpp>

//==============================================================================
// FORWARD DECLARATIONS
//==============================================================================

/// Handle for Quanser devices. Must be in global namespace.
typedef struct tag_card* t_card;

namespace mahi {
namespace daq {

class QuanserWatchdog;
class QuanserAI;
class QuanserAO;
class QuanserDI;
class QuanserDO;
class QuanserDIO;
class QuanserEncoder;
class QuanserPwm;

//==============================================================================
// TYPDEFS
//==============================================================================

/// Typedef for Quanser device handle
typedef t_card QuanserHandle;

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Encapsulates a generic Quanser DAQ
class QuanserDaq : public DaqBase {
public:
    /// Default constructor
    QuanserDaq(const std::string& card_type,
               unsigned int id,
               QuanserOptions options = QuanserOptions());

public:

    /// Overloaded version of set_options() that allows passing in new options
    bool set_options(const QuanserOptions& options);

    /// Gets a copy of the QOptions of this QuanserDaq
    QuanserOptions get_options();

public:
    /// Determines how many of a specifc QuanserDaq are currently connected to the host
    static std::size_t get_qdaq_count(const std::string& card_type);

    /// Gets the string message corresponding to a Quanser error number
    static std::string get_quanser_error_message(int error, bool format = true);

protected:

    virtual bool on_open() override;
    virtual bool on_close() override;

protected:

    friend class QuanserWatchdog;
    friend class QuanserAI;
    friend class QuanserAO;
    friend class QuanserDI;
    friend class QuanserDO;
    friend class QuanserDIO;
    friend class QuanserEncoder;
    friend class QuanserPwm;

    std::string card_type_;  ///< The card type string, per Quarc API
    unsigned int id_;              ///< The ID# of this Q8 USB
    QuanserHandle handle_;   ///< Internal handle to the Quanser device
    QuanserOptions options_; ///< The board specific options of this Q8 USB

private:

    QuanserDaq( const QuanserDaq& ) = delete; // non construction-copyable
    QuanserDaq& operator=( const QuanserDaq& ) = delete; // non copyable
};

} // namespace daq
} // namespace mahi
