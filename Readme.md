# Eye Tracker Validation

This software is a free, open-source tool which can be used to validate the
accuracy and precision of an eye tracker system. The tool works on both Windows
and Linux systems ,and should work with OS-X, thouugh this hasn't been tested.
It has been designed to allow new trackers to be easily added and tested with
minimal coding; the demo programs included with your eye tracker will probably
contain all the code you need.

If you use this tool and add a new tracker, please submit a pull request to
allow others to make use of your work.

## Dependencies
The number of dependencies has intentionally been kept small to allow the code
to be built and run with minimal fuss. The build is managed by `cmake`, with UI
components managed by `OpenGL`. Both of these are widely available.

### Windows
On Windows, the easiest way to get up and running is to open the folder with
Visual Studio. If you don't have `cmake` or a C++ compiler installed, they can
be added using the Visual Studio Installer. `OpenGL` is available in Windows by
default.

### Linux
On Linux, the only additional dependencies are the OpenGL and X11 development
libraries. To install on a debian-based system, use the following:

```sudo apt-get install -y libgl1-mesa-dev libglu1-mesa-dev libxi-dev```

If you don't have `cmake` or C/C++ compilers installed, you'll need these too:

```sudo apt-get install -y build-essential cmake```

## Build
Since this project uses `cmake`, your build tool can change depending on your
environment. The default settings are listed below.

### Windows
From Visual Studio, simply build the target `TrackerValidation`. This will
generate the build files (`ninja` by default) and run them. Nothing else to do.

### Linux
`cmake` will generate `Makefile` files by default. From the root directory, run
the following to generate the files:

```cmake .```

To compile, run:

```make```

Note that if you don't add/remove any source code, libraries or other
dependencies, then you only need to run `make` after each code change. If you
_do_ change dependencies, you'll need to run `cmake .` from the root directory
again.

# Design Overview

## Data and Process Flow
![High-level Overview](./docs/diagrams/Tracker_validation_flowchart.svg)

## Object Relations
![Object Relations Overview](./docs/diagrams/Tracker_validation_uml.svg)