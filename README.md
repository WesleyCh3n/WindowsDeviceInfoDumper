# Device Info Dumper

This is a minimum c++ example to dump information (hardware ID, manufacturer etc..)
about installed device using Windows SDK API.

## Build Requirements

Tested with

- Windows 10 21H2 (1904 build)
- MSVC 2019
- CMake 3.21.3

## How to Compile?

Clone the repository

```bash
git clone https://github.com/WesleyCh3n/WindowsDeviceInfoDumper.git
cd WindowsDeviceInfoDumper
```

CMake the project

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

Then the executable will be in `./build/Release/` directory.

## Usage

```bash
./devInfoDumper
```

Then there are both `stdout` and `file.json` about device information.

## Note

If json value showing `[Error: #]`, `#` is [windows system error code](https://learn.microsoft.com/en-us/windows/win32/debug/system-error-codes--0-499-).
Go check out doc to see what happened.
