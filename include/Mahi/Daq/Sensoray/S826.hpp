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

#include <Mahi/Daq/Daq.hpp>
#include <Mahi/Daq/Sensoray/S826AI.hpp>
#include <Mahi/Daq/Sensoray/S826AO.hpp>
#include <Mahi/Daq/Sensoray/S826DIO.hpp>
#include <Mahi/Daq/Sensoray/S826Encoder.hpp>
#include <Mahi/Daq/Sensoray/S826Watchdog.hpp>

namespace mahi {
namespace daq {

class S826 : public Daq {
public:

    S826(int board = 0);
    ~S826();

    virtual bool on_open() override;
    virtual bool on_close() override;
    virtual bool on_enable() override;
    virtual bool on_disable() override;

    /// Returns the current board time
    util::Time get_time() const;

public:

    /// Returns string message for a givenn S826 API error code.
    static std::string get_error_message(int error);

public:

    S826AI  AI;
    S826AO  AO;
    S826DIO DIO;
    S826Encoder encoder;
    S826Watchdog watchdog;

private:

    friend class S826AI;
    friend class S826AO;
    friend class S826DIO;
    friend class S826Encoder;
    friend class S826Watchdog;

    int board_; ///< the S826 baord identification number

    S826( const S826& ) = delete; // non construction-copyable
    S826& operator=( const S826& ) = delete; // non copyable
};

} // namespace daq
} // namespace mahi