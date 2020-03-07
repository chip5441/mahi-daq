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

#include <Mahi/Daq.hpp>
#include <Mahi/Util.hpp>

using namespace mahi::daq;
using namespace mahi::util;

ctrl_bool stop(false);

bool handler(CtrlEvent event) {
    if (event == CtrlEvent::CtrlC)
        stop = true;
    return true;
}

int main() {

    // register CTRL-C handler
    register_ctrl_handler(handler);

    //==============================================================================
    // CONSTUCT/OPEN/CONFIGURE
    //==============================================================================

    // create default Q28 USB object (all channels enabled)
    Q2Usb q2;
    if (!q2.open())
        return 1;

    //==============================================================================
    // ENABLE
    //==============================================================================

    // ask for user input
    prompt("Press ENTER to open and enable Q8 USB.");
    // enable Q8 USB
    if (!q2.enable())
        return 1;

    //==============================================================================
    // ENCODER
    //==============================================================================

    // ask for user input
    prompt("Connect an encoder to channel 0 then press ENTER to start test.");
    // create 10 Hz Timer
    Timer timer(milliseconds(100));
    // start encoder loop
    while (timer.get_elapsed_time() < seconds(5) && !stop) {
        q2.update_input();
        print_var(q2.encoder.get(0));
        timer.wait();
    }

    //==============================================================================
    // ANALOG INPUT/OUTPUT
    //==============================================================================

    // ask for user input
    prompt("Connect AO0 to AI0 then press ENTER to start test.");
    // create Waveform
    Waveform wave(Waveform::Sin, seconds(0.25), 5.0);
    // start analog loopback loop
    timer.restart();
    while (timer.get_elapsed_time() < seconds(5) && !stop) {
        q2.update_input();
        print_var(q2.AI.get(0));
        double voltage = wave.evaluate(timer.get_elapsed_time());
        q2.AO.set(0, voltage);
        q2.update_output();
        timer.wait();
    }

    //==============================================================================
    // DIGITAL INPUT/OUTPUT
    //==============================================================================

    // ask for user input
    prompt("Connect DIO0 to DIO7 then press ENTER to start test.");
    // set directions
    q2.DIO.set_direction(7, Out);
    Logic signal = High;
    // start analog loopback loop
    timer.restart();
    while (timer.get_elapsed_time() < seconds(5) && !stop) {
        q2.DIO.update();
        print_var((int)q2.DIO[0]);
        signal = (Logic)(High - signal);
        q2.DIO[7] = signal;
        q2.DIO.update();
        timer.wait();
    }

    //==============================================================================
    // LED
    //==============================================================================

    // ask for user input
    prompt("Press enter then press any key to turn on LED.");
    // make sure LED Mode is set to User
    QuanserOptions options;
    options.set_led_mode(QuanserOptions::LedMode::User);
    q2.set_options(options);
    timer.restart();
    while (timer.get_elapsed_time() < seconds(10) && !stop) {
        if (get_key_nb())
            q2.set_led(High);
        else
            q2.set_led(Low);
        q2.update_output();
        timer.wait();
    }

    //==============================================================================
    // DISABLE
    //==============================================================================

    // ask for user input
    prompt("Press Enter to disable the Q2 USB.");
    // disable Q8 USB
    q2.disable();
    // close Q8 USB
    q2.close();

    return 0;
}
