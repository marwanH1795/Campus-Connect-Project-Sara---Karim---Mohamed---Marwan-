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

Run the server first on one computer:

```bash
./server.exe
```

Then run the app on each computer:

```bash
./app.exe
```

For two teammates to chat from different computers, both apps must connect to the
same server. On the computer running `server.exe`, find its local network IP
address, for example `192.168.1.25`, then enter that IP in the app's server field
on the other computer. `127.0.0.1` only connects to the current computer, so it
works for local testing but not for a teammate on another device.

If the app cannot connect, allow `server.exe` through Windows Firewall and keep
port `12345` selected in the login screen.

## Features

* Public chat
* Group creation and joining
* Group messaging
* Qt Designer UI integration
* JSON-based client-server communication
