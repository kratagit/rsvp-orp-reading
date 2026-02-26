# RSVP ORP Speed Reader

A fast, lightweight, and efficient speed-reading application written in C using the [Raylib](https://www.raylib.com/) library. 

This application uses the **RSVP** (Rapid Serial Visual Presentation) method combined with the **ORP** (Optimal Recognition Point) technique. Instead of moving your eyes across a page, words are flashed on the screen one by one, with a specific letter highlighted in red (the ORP). This minimizes eye movement and allows you to read at incredibly high speeds (up to 1000+ WPM) with less fatigue.

The ORP calculation in this app follows the "Golden Rule" established by Spritz research.

## Features
- **RSVP & ORP Technology**: Read faster by focusing on a single point.
- **Custom Text Loading**: Automatically loads text from a `tekst.txt` file.
- **Full UTF-8 Support**: Fully supports Polish characters (ą, ć, ę, ł, ń, ó, ś, ź, ż) and other UTF-8 symbols.
- **Adjustable Speed**: Change Words Per Minute (WPM) on the fly using the keyboard or mouse wheel.
- **High-Quality Rendering**: Uses SDF (Signed Distance Field) fonts and MSAA for crisp, clear text at large sizes.

## Controls
- **Spacebar / Left Mouse Click**: Play / Pause the text.
- **Up Arrow**: Increase speed by 25 WPM.
- **Down Arrow**: Decrease speed by 25 WPM.
- **Mouse Wheel Up**: Increase speed by 10 WPM.
- **Mouse Wheel Down**: Decrease speed by 10 WPM.
- **Right Arrow**: Manually skip to the next word (when paused).
- **Left Arrow**: Manually go back to the previous word (when paused).

---

## Prerequisites

To build and run this project, you need a C compiler and CMake. The Raylib library is automatically downloaded and built by CMake, so you don't need to install it manually.

### Linux (Ubuntu/Debian/Arch/Fedora)

1. **Install build tools and dependencies:**
   
   *For Ubuntu/Debian:*
   ```bash
   sudo apt update
   sudo apt install build-essential cmake git libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev libwayland-dev libxkbcommon-dev
   ```

   *For Arch Linux / CachyOS:*
   ```bash
   sudo pacman -S base-devel cmake git wayland libxkbcommon
   ```

### Windows

1. **Install MSYS2 (Recommended for GCC):**
   - Download and install [MSYS2](https://www.msys2.org/).
   - Open the **MSYS2 UCRT64** terminal and install the toolchain and CMake:
     ```bash
     pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja git
     ```
   - Add `C:\msys64\ucrt64\bin` to your Windows System `PATH` environment variable.

2. **Alternative: Visual Studio (MSVC):**
   - Install [Visual Studio Community](https://visualstudio.microsoft.com/) with the "Desktop development with C++" workload (this includes CMake and the MSVC compiler).

---

## How to Build and Run

### 1. Clone the repository
```bash
git clone <your-repo-url>
cd rsvp-orp-reading
```

### 2. Prepare your text
Create or edit the `tekst.txt` file in the root directory of the project. Paste any text you want to practice reading. The application requires this file to run.

### 3. Build the project

**On Linux / Windows (MSYS2):**
```bash
# Create a build directory
mkdir build
cd build

# Configure the project with CMake
cmake ..

# Compile the project
make
```

**On Windows (Visual Studio / MSVC):**
Open the "x64 Native Tools Command Prompt for VS" and run:
```cmd
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### 4. Run the application

**On Linux:**
```bash
./main
```

**On Windows:**
```cmd
main.exe
# or if using MSVC:
Release\main.exe
```

*Note: The `CMakeLists.txt` is configured to automatically copy `Roboto-Regular.ttf` and `tekst.txt` to your build directory every time you compile, ensuring the app always has access to the latest text and font.*

## Customization
If you want to change the font, simply replace `Roboto-Regular.ttf` with another TrueType Font (`.ttf`) file and update the filename in `main.c` and `CMakeLists.txt`.