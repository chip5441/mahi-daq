<p align="center">
<img src="https://github.com/mahilab/mahi-daq/blob/master/mahi-daq.png" width="256"> 
</p>

## mahi-daq

This library provides a unfied hardware abstraction layer for commercial multifunction DAQs from Quanser, Sensoray, and National Instruments. It can be used for anything ranging from taking simple samples, to complex real-time controls. 

### Integration

The library is small and intended to be used with CMake's `FetchContent`:

```cmake
include(FetchContent) 
FetchContent_Declare(mahi-daq GIT_REPOSITORY https://github.com/mahilab/mahi-daq.git) 
FetchContent_MakeAvailable(mahi-daq)

add_executable(my_app "my_app.cpp")
target_link_libraries(my_app mahi::daq)
```

That's it! You should also be able to install or use the library as a git-submodule + CMake subdirectory if you prefer.

### Currently Supported Devices

- Quanser Q8-USB 
- Quanser QPID
- Quanser Q2
- Sensoray S826
- NI myRIO

### Example Usage

```cpp
Q8Usb q8;
q8.enable();

// synchronized reads
q8.read_all();
bool di0 = q8.DI[0];
double ao5 = q8.AI[5];
int enc3 = q8.encoder[3];

// synchronized writes
q8.DO[7] = true;
q8.AO[2] = 1.23;
q8.write_all();

// per module reads/writes
q8.AI.read();
q8.AO[0] = q8.AI[0];
q8.AO.write();

// per channel channel read/writes
di0 = q8.DI.read(0);
q8.DO.write(0,di0);

// automatic handling of shared pins
q8.PWM.set_channels({0,1,2,3}); // converts DO 0:3 to PWM outputs
q8.PWM.frequencies.write(0, 20000);
q8.PWM.write(0, 0.75);
```

### Requirments 

- C++11 compiler
- Quanser only: [Quanser HIL SDK](https://github.com/quanser/hil_sdk_win64)
- Sensoray S826 only: [S826 SDK](http://www.sensoray.com/PCI_Express_digital_output_826.htm)
- myRIO only: [GNU C & C++ Compilers for ARMv7 Linux](http://www.ni.com/download/labview-real-time-module-2018/7813/en/)
