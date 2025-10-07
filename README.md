# **This is a lab project for CSE 4108 Lab**

<br>

## **Here's Our Introduction:**

<br>

![image alt](https://github.com/KraKEn-bit/CSE_Project_SP2/blob/main/Image/PresentationWindow%2010_8_2025%2012_13_54%20AM.png?raw=true)


# **What Libraries did we use for our simulation game**?

Ans: ONLY **RAYLIB**  : ( 

### **Here's the official Raylib website from where you can learn about raylib library, its headers,in built functions etc**

[RAYLIB OFFICIAL](https://www.raylib.com/)


# **RAYLIB Installation process for C**

# Install & use **raylib** in VS Code (C) — Step-by-step (repo-ready)

> A beginner-friendly, copy-pasteable guide you can add to your GitHub repo. Covers Windows (MSYS2), macOS (Homebrew), and GNU/Linux (Ubuntu). Includes an example program, Makefile, and VS Code tasks/launch configs.

---

## TL;DR

1. Install a C compiler & tools (MSYS2 on Windows / Homebrew on macOS / `build-essential` & `cmake` on Linux).
2. Install `raylib` (MSYS2 `pacman`, `brew install raylib`, or build-from-source with CMake on Linux).
3. Use `pkg-config` to compile: `gcc src/main.c -o bin/hello $(pkg-config --cflags --libs raylib)`.
4. Open the folder in VS Code, press `Ctrl+Shift+B` to build (or run the provided task).

---

## Repository layout (suggested)

```
my-raylib-project/
├─ .vscode/
│  ├─ tasks.json
│  └─ launch.json
├─ src/
│  └─ main.c
├─ bin/       # build outputs (add to .gitignore if you want)
├─ build/     # optional CMake build dir
├─ Makefile
└─ README.md  # this file (INSTALL guide)
```

---

## Prerequisites (common)

* Git
* VS Code
* C/C++ extension for VS Code (ms-vscode.cpptools)
* A C compiler: GCC/Clang (on Windows use MSYS2 MinGW64 toolchain)
* `pkg-config` (makes linking with raylib easy)
* `cmake` (optional but useful when building raylib from source)

---

## 1) Windows — using **MSYS2 (recommended for VS Code + gcc)**

1. Install MSYS2: [https://www.msys2.org](https://www.msys2.org) (choose the installer for Windows).
2. Run **MSYS2 MinGW 64-bit** shell (not the plain MSYS shell) from the Start menu.
3. Update packages (follow MSYS2 update instructions):

```bash
# in MSYS2 MinGW64 shell
pacman -Syu   # may require closing and reopening the shell
pacman -Su
```

4. Install the toolchain, pkg-config, CMake and raylib:

```bash
pacman -S --needed base-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-pkg-config mingw-w64-x86_64-cmake mingw-w64-x86_64-raylib
```

5. Build your example (in the MinGW64 shell) using `pkg-config`:

```bash
mkdir -p bin
gcc src/main.c -o bin/hello $(pkg-config --cflags --libs raylib)
./bin/hello.exe
```

> Notes: run compilation commands in the **MINGW64** shell so that `gcc`, `pkg-config`, and the `raylib` package from pacman are visible.

---

## 2) macOS — Homebrew (simple)

1. Install Homebrew (if you don’t have it):

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

2. Install raylib:

```bash
brew install raylib
```

3. Build your example:

```bash
mkdir -p bin
gcc src/main.c -o bin/hello $(pkg-config --cflags --libs raylib)
./bin/hello
```

---

## 3) GNU/Linux (Ubuntu / Debian) — build or install

**Option A — install from source (recommended when package not available):**

```bash
# install build tools + dependencies
sudo apt update
sudo apt install -y build-essential git cmake pkg-config \
    libasound2-dev libpulse-dev libx11-dev libxcursor-dev libxrandr-dev libxi-dev libgl1-mesa-dev libudev-dev

# clone and build raylib
git clone https://github.com/raysan5/raylib.git
cd raylib
mkdir build && cd build
cmake -DBUILD_SHARED_LIBS=ON ..
make -j$(nproc)
sudo make install
```

After `sudo make install`, `pkg-config` should find `raylib`, so you can compile your program as below.

**Option B — distro package**: some distros/package managers provide raylib packages (use your distro's package manager). If available, use that for easier setup.

Build your example (after install):

```bash
mkdir -p bin
gcc src/main.c -o bin/hello $(pkg-config --cflags --libs raylib)
./bin/hello
```

---

## Example: `src/main.c` (copy-paste)

```c
#include "raylib.h"

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
```

---

## Makefile (simple cross-platform using pkg-config)

```makefile
CC = gcc
SRC = src/main.c
OUT = bin/hello

all: $(OUT)

$(OUT): $(SRC)
	@mkdir -p $(dir $(OUT))
	$(CC) $(SRC) -o $(OUT) $(shell pkg-config --cflags --libs raylib)

clean:
	rm -rf bin build

.PHONY: all clean
```

> Use `make` to build. On Windows run `make` inside the MINGW64 shell where `make` and `pkg-config` point to the MinGW packages.

---

## VS Code: tasks.json (build) & launch.json (run / debug)

Create `.vscode/tasks.json` (this runs the `make` file):

```json
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "Build (make)",
      "type": "shell",
      "command": "make",
      "group": {
        "kind": "build",
        "isDefault": true
      },
      "presentation": {
        "reveal": "always"
      }
    }
  ]
}
```

Create `.vscode/launch.json` (GDB/LLDB based debug; adjust `program` path if needed):

```json
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "Debug raylib (GDB)",
      "type": "cppdbg",
      "request": "launch",
      "program": "${workspaceFolder}/bin/hello",
      "args": [],
      "stopAtEntry": false,
      "cwd": "${workspaceFolder}",
      "environment": [],
      "externalConsole": true,
      "MIMode": "gdb",
      "miDebuggerPath": "gdb",
      "setupCommands": [
        { "description": "Enable pretty-printing for gdb", "text": "-enable-pretty-printing", "ignoreFailures": true }
      ]
    }
  ]
}
```

> On Windows (MSYS2) make sure `gdb` is installed (`pacman -S mingw-w64-x86_64-gdb`) and launch VS Code from the MinGW64 shell so environment PATHs resolve. On macOS, you might use `lldb` instead and change `MIMode`.

---

## .gitignore (suggested)

```
/bin
/build
/.vscode/launch.json   # contains local debugger paths — keep out of repo if you like
*.o
```

---

## Commit & push to GitHub

```bash
git init
git add .
git commit -m "Add raylib setup guide + example"
# create repo on GitHub, then:
git branch -M main
git remote add origin https://github.com/<your-username>/<your-repo>.git
git push -u origin main
```

---

## Troubleshooting & tips

* If `pkg-config --cflags --libs raylib` returns nothing, raylib is not properly installed or `PKG_CONFIG_PATH` is missing.
* On Windows always use the **MINGW64** shell for MinGW toolchain packages.
* If you get linker errors about `-lGL` or `-lopengl32`, make sure your platform's OpenGL dev packages are installed.
* Prefer building raylib from source on Linux to ensure `raylib.pc` is installed and `pkg-config` works.

---

## Where to learn more

* Official raylib repository & wiki: [https://github.com/raysan5/raylib](https://github.com/raysan5/raylib)
* Raylib official site & examples: [https://www.raylib.com/](https://www.raylib.com/)

---

If you want, I can also generate the `src/main.c`, `Makefile`, and `.vscode/` files and put them here so you can copy them directly into your repo. Let me know which OS you are using (Windows / macOS / Linux) and I will create files tailored for that environment.

Good luck — enjoy learning raylib! 🎮
