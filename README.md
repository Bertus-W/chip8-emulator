# CHIP-8 Emulator

This CHIP-8 emulator was developed following [this guide](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM) and has been tested using [these test scripts](https://github.com/Timendus/chip8-test-suite).

## Requirements

This program uses the `raylib` library. To install it, follow the instructions below for your operating system.

### Installing `raylib`

#### macOS

```bash
brew install raylib
```

#### Linux

For Ubuntu-based systems:

```bash
sudo add-apt-repository ppa:texus/raylib
sudo apt install libraylib5-dev
```

For other Linux distributions, check the official installation instructions [here](https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux).

#### Windows

You can download the latest Windows installer or follow the setup instructions [here](https://github.com/raysan5/raylib/wiki/Working-on-Windows).

## Build Instructions

After installing `raylib`, you can build the project by running the following command in the project directory:

```bash
make
```

Make sure `raylib` is properly installed and available for your compiler.
