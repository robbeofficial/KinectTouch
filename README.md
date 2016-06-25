KinectTouch
==
Turns any surface into a giant touchpad using kinect

[![kinect - build a TUIO multitouch pad on any surface](http://img.youtube.com/vi/4zXtV66cFDY/0.jpg)](http://www.youtube.com/watch?v=4zXtV66cFDY)

Prerequisites
==
  - OpenNI 1.x, NITE and SensorKinect
    - [Bundelded download](https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/simple-openni/OpenNI_NITE_Installer-Linux64-0.27.zip) as part of [SimpleOpenNI](https://code.google.com/archive/p/simple-openni/) available
    - Ubuntu installation
        ```
        wget https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/simple-openni/OpenNI_NITE_Installer-Linux64-0.27.zip
        unzip OpenNI_NITE_Installer-Linux64-0.27.zip
        cd OpenNI_NITE_Installer-Linux64-0.27/
        sudo ./OpenNI-Bin-Dev-Linux-x64-v1.5.4.0/install.sh
        sudo ./NITE-Bin-Dev-Linux-x64-v1.5.2.21/install.sh
        sudo ./kinect/Sensor-Bin-Linux-x64-v5.1.2.1/install.sh
        ```
  - [OpenCV](http://opencv.org/)
    - Ubuntu installation
        ```
        sudo apt-get libopencv-dev
        ```
  - [CMake](https://cmake.org/)
    - Ubuntu installation
        ```
        sudo apt-get install cmake
        ```

Building and Running
==
```bash
mkdir build
cd build
cmake ..
make
```

```bash
./KinectTouch
```

TODOs
==
 - Integrate [TUIO](https://github.com/mkalten/TUIO11_CPP) as a submodule
 - Integrate [OpenNI](https://github.com/OpenNI) and [SensorKinect](https://github.com/avin2/SensorKinect) submodules or switch to [libfreenect](https://github.com/OpenKinect/libfreenect)


