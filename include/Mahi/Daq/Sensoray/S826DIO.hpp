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

#include <Mahi/Daq/InputOutput.hpp>

namespace mahi {
namespace daq {

class S826;

class S826DIO : public DigitalInputOutput {
public:

    bool update() override;
    bool update_channel(ChanNum channel_number) override;
    bool set_direction(ChanNum channel_number, Direction direction) override;

private:
    friend class S826;

    /// Private constructor used by S826
    S826DIO(S826& s826);

    /// Called by S826 when it opens
    bool on_open();

private:
    S826& s826_;

    S826DIO( const S826DIO& ) = delete; // non construction-copyable
    S826DIO& operator=( const S826DIO& ) = delete; // non copyable
};

} // namespace daq
} // namespace mahi