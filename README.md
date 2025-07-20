# TCP Chat App

<img width="1024" height="597" alt="TCP-CHAT-APP" src="https://github.com/user-attachments/assets/913f8c38-1e6d-4d13-960a-9f7608e78aa2" />


A low-level real-time chat system built in **C** using **raw TCP sockets**, designed for **Linux** environments. This project implements a multi-client chat architecture using custom socket logic, enabling direct peer-to-peer communication via a central server.



## 🌟 Features

- **Socket Lifecycle Management**: Complete handling of bind, listen, accept, connect, read, write, and graceful shutdown.
- **Multi-Client Support**: Each client connection is handled in a separate thread using `pthread`, allowing concurrent communication.
- **Custom Protocol Design**: Basic message framing for clarity in network transmission.
- **Thread-safe Messaging**: Input/output handled with synchronization primitives to prevent race conditions.
- **Connection Robustness**: Server handles client disconnections and reconnections without crashing.
- **Real-time I/O**: Integrated `select()` for non-blocking I/O and efficient multiplexing.
- **Clean Project Structure**: Modular build system using **CMake**, with separate directories for server, client, and utility logic.

---

## 📁 Project Structure

```bash
SOCKTALK/
├── CMakeLists.txt                 ✅ root CMakeLists
├── build/                         
├── SocketUtil/
│   ├── socketutil.c
│   ├── socketutil.h
│   └── CMakeLists.txt            ✅ builds shared library
├── SocketClient/
│   ├── main.c
│   └── CMakeLists.txt            ✅ builds client and links SocketUtil
└── SocketServer/
    ├── main.c
    └── CMakeLists.txt            ✅ builds server and links SocketUtil


```
---

## How to Build and Run

```bash

# Clone the repository
git clone https://github.com/ojasks/TCP-Chat-App.git
cd TCP-Chat-App

# Create and enter build directory
mkdir -p build && cd build

# Generate build files and compile
cmake ..
make

```

## Run the Server


```bash


./SocketServer/SocketServer


```

## Run the Client (in a new terminal)


```bash


./SocketClient/SocketClient <server_ip> <port>


 ```


## Build Instructions


```bash

# From project root
rm -rf build
mkdir build
cd build
cmake ..
make

```



---

## 🧠 Learnings & Takeaways

- How TCP ensures reliable, in-order byte streams.
- How to implement multi-threaded concurrency and avoid race conditions.
- How to handle real-world issues like partial reads/writes and socket timeouts.
- How to build modular systems and separate client/server responsibilities.
- How to debug low-level systems code using tools like `valgrind` and `gdb`.

---

## 📈 In Process.. (Upcoming Enhancements)

As I keep learning more about systems and network programming, I’ll be adding:

- [🔐] **Encrypted Communication** using OpenSSL/libsodium
- [🗃️] **Chat History Logging** per client session
- [🎨] **Colored Terminal UI** using ncurses
- [🔑] **Client Authentication** (login & registration)
- [🌐] **IPv6 Compatibility** & dual-stack support
- [🧰] **CLI Configurations** for host/port customization
- [💬] **Command Parsing** (`/list`, `/quit`, `/msg`)
- [⛔] **Graceful Server Shutdown** for all active threads
- [📶] **Packet Loss Simulation & Retry Logic**
- [🧪] **Unit Tests & GitHub Actions CI**

---
 


### 🔨 Build Instructions

```bash

# Clone the repo
git clone https://github.com/ojasks/TCP-Chat-App.git
cd TCP-Chat-App

# Build the project using CMake
cmake -B build
cmake --build build


```



## Contributions

Feel free to fork the project, report bugs, or suggest enhancements.

```bash

# Create a new branch for your changes
git checkout -b your-feature-name
# Submit a PR with a proper description

```
