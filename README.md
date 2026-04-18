## Build and Run

### Requirements

* Qt 6.10.2 or compatible Qt 6
* Qt Creator or CMake
* MinGW 64-bit toolchain on Windows

### Build with Qt Creator

1. Open the project folder in Qt Creator.
2. Select the `Desktop Qt 6.10.2 MinGW 64-bit` kit.
3. Configure the project.
4. Build the project.

### Build with CMake

```bash
mkdir build
cd build
cmake -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:/Qt/6.10.2/mingw_64" ..
cmake --build .
```

### Run

Run the server first:

```bash
./server.exe
```

Then run the app:

```bash
./app.exe
```

## Features

* Public chat
* Group creation and joining
* Group messaging
* Qt Designer UI integration
* JSON-based client-server communication
