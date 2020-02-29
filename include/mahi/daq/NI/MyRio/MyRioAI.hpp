// MIT License
//
// Copyright (c) 2019 Mechatronics and Haptic Interfaces Lab - Rice University
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
#include <mahi/daq/Input.hpp>
#include <MEL/Core/NonCopyable.hpp>

namespace mahi {
namespace daq {

class MyRioConnector;

/// myRIO Analog Input Module
class MyRioAI : public AnalogInput, NonCopyable {
public:

    /// Updates a single channel
    bool update_channel(ChanNum channel_number) override;

private:

    friend class MyRioConnector;

    MyRioAI(MyRioConnector& connector, const ChanNums& channel_numbers);

private:

    const MyRioConnector& connector_; ///< connector this module is on
};

} // namespace daq
} // namespace mahi
