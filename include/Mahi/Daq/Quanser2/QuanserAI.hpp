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
#include <Mahi/Daq/Io.hpp>
#include <Mahi/Daq/Quanser2/QuanserHandle.hpp>

namespace mahi {
namespace daq {

class QuanserDaq;

class QuanserAI : public Fused<AIModule,QuanserDaq> {
public:
    QuanserAI(QuanserDaq& d, QuanserHandle& h, const ChanNums& chs);
    Fused<WriteBuffer<Voltage>, QuanserAI> min_values;
    Fused<WriteBuffer<Voltage>, QuanserAI> max_values;
private:
    QuanserHandle& m_h;
};

} // namespace daq
} // namespace mahi