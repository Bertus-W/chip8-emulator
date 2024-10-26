# CHIP-8 Emulator

This CHIP-8 emulator was developed based on the specifications provided in [this technical reference guide](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM). It has been thoroughly tested using [this set of test programs](https://github.com/Timendus/chip8-test-suite) to ensure accurate emulation of the CHIP-8 system.

## Requirements

To run this project, you need to install the `raylib` graphics library. The installation steps vary depending on your operating system, as described below.

### Installing `raylib`

#### macOS

To install `raylib` on macOS, use Homebrew:

```bash
brew install raylib
```

#### Linux

For Ubuntu-based systems, run the following commands:

```bash
sudo add-apt-repository ppa:texus/raylib
sudo apt install libraylib5-dev
```

For other Linux distributions, refer to the official installation guide [here](https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux).

#### Windows

For Windows, download the latest installer or follow the setup instructions provided [here](https://github.com/raysan5/raylib/wiki/Working-on-Windows).

## Building the Project

Once `raylib` is installed, you can build the project by running the following commands in the project directory:

```bash
cmake -B build
cmake --build build
```

Make sure that `raylib` is correctly installed and accessible by your compiler.

`CMake` will automatically generate the appropriate build files for your platform. You will need a compatible C++ compiler and `CMake` version 3.15 or later installed.

## Additional Notes

- Ensure that your compiler supports C++11 or later.
- The emulator has been tested across multiple platforms to verify compatibility.

With everything set up, you should be ready to run the CHIP-8 emulator and explore its functionality!


