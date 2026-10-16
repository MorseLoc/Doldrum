# Doldrum

A C++20 / SDL3 roguelike prototype built with CMake and Visual Studio.

## Requirements
- Windows 10/11 and Visual Studio 2022 or newer with **Desktop development with C++** and CMake support.
- Git installed and internet access for initial SDL3 download (unless SDL3 is already installed).

## Open and run
1. Extract the folder and open Visual Studio.
2. Select **File > Open > Folder...** and select this directory (the one containing `CMakeLists.txt`).
3. Select the `windows-debug` configure preset, let CMake finish, and choose the **Doldrum** startup target.
4. Build and run with F5 or Debug > Start Debugging.

If Visual Studio does not detect Git, install Git for Windows. First CMake configure can take longer because it fetches and builds SDL3. If using an installed SDL3, make sure CMake can find its package.

### Controls
- **WASD**: move (diagonal speed normalized)
- **Arrow keys**: shoot (four directions, held fire)
- **Esc**: exit

### Command-line alternative
From a Developer PowerShell with CMake and Ninja:
```powershell
cmake --preset windows-debug
cmake --build --preset windows-debug
```

## GitHub setup
Create an empty private or public repository named `Doldrum` on GitHub (do not initialize with README). Open a terminal at this directory and run:
```powershell
git init
git add .
git commit -m "Initial Doldrum CMake SDL3 prototype"
git branch -M main
git remote add origin https://github.com/YOUR_USERNAME/Doldrum.git
git push -u origin main
```
GitHub authentication may open a browser prompt. Do not paste access tokens into chat.

## Current status
Build 001: test arena, free movement and four-directional projectile shooting. There are no room TXT loaders, enemies, card system, saves, or final art yet. The preview uses simple rectangles deliberately. See `docs/ROADMAP.md` for the next step.
