## My own readme 

First of all in Berkeley Sockets
we would create our own socket and currently we are writing the client side of the application
so as are we writing a client there must. be a server sittling idle waiting to listen for incoming connection
we will write the server side later on
port no will be the number that the application will acquire form the OS
so that other side could connect to it
ports from 0 to 1000 are reserved

### TCP = Transmission Control Protocol
reliable, connection-based protocol built on top of IP (Internet Protocol).
Key Features of TCP:
1.Guarantees delivery of packets in order
2.Detects lost or corrupted packets and retransmits
3Provides a stream-oriented connection (like a pipe between two systems)
4.Handles flow control, congestion control, and error checking

### Why TCP in chat apps?
You need messages to arrive in full and in order
You want the connection to stay alive (like a phone call)
Think of TCP as a virtual wire between two systems — you write on one end, it comes out the other, no mess.

### POSIX Threads (pthreads)?  POSIX = Portable Operating System Interface
 standard threading library on UNIX-based systems (Linux, macOS)
 Let’s you run multiple tasks in parallel within the same process
 a family of standards for UNIX systems — including threads.

### How POSIX threads are used in your code?
pthread_t id ;
pthread_create(&id, NULL, listenAndPrint, socketFD);

* listenAndPrint= Function that this new thread will run
* socketFD = Passed as an argument (shared resource)
So now:
main() continues with readConsoleEntriesAndSendToServer()
While in parallel, listenAndPrint() runs in a background thread.

### SOCKETclient -main.c code explanation
a basic TCP client app using POSIX threads to handle sending and receiving messages concurrently, likely connecting to a TCP chat server on 127.0.0.1:2000

Function by Function Breakdown()
main()
* int socketFD = createTCPIpv4Socket();
* struct sockaddr_in *address = createIPv4Address("127.0.0.1", 2000);
* connect(socketFD,address,sizeof (*address));

Creates a TCP socket
Builds an IPv4 address struct for localhost:2000
Attempts to connect the socket to the server
If successful, it:
Starts a listener thread to receive messages from the server
Reads user input from console and sends to server
Closes socket at the end

readConsoleEntriesAndSendToServer(socketFD)
* getline(&name,&nameSize,stdin);  // ask for user's name
* sprintf(buffer,"%s:%s",name,line);  // formats message like "Ojas:hello"
* send(socketFD, buffer, strlen(buffer), 0);

If the user types "exit", it breaks and stops sending

startListeningAndPrintMessagesOnNewThread(socketFD)
* pthread_create(&id,NULL,listenAndPrint,socketFD);

Purpose: Starts a new thread for listening to server responses.
This ensures sending and receiving can happen concurrently
 a new thread is spwaned that runs listenAndPrint

listenAndPrint(socketFD)
* recv(socketFD, buffer, 1024, 0);

Continuously receive data from server and print it.
Blocks until something is received
If data is received, prints it, If recv() returns 0 (socket closed), breaks and closes the socket


now we are going to work on the Socketserver main.c so that it could listen to our socket
### multi-client TCP chat server.

Listens for incoming connections on port 2000.
Accepts each connection and spawns a new thread to handle that client.
Receives messages from one client and broadcasts them to all others.
When a client disconnects, it closes that socket.

Key Parts of the Code
1. main()
Creates a TCP socket: createTCPIpv4Socket()
Binds it to port 2000: bind()
Starts listening: listen()
Starts accepting connections: startAcceptingIncomingConnections()

2. startAcceptingIncomingConnections(int serverSocketFD)
Infinite loop: waits for new clients.
Each accepted client gets added to acceptedSockets[]
Starts a new thread to handle receiving messages from that client.

3. receiveAndPrintIncomingDataOnSeparateThread()
Starts a new thread using pthread_create() to run receiveAndPrintIncomingData().

4. receiveAndPrintIncomingData(int socketFD)
Listens for data from one client using recv().
Prints message to server console.
Broadcasts the same message to all other clients using send().

5. sendReceivedMessageToTheOtherClients(char *buffer, int socketFD)
Loops through the acceptedSockets list and sends the message to all clients except the sender.

6. acceptIncomingConnection()
Calls accept() to wait for a new client.
Wraps details into a custom AcceptedSocket struct for easier handling.



### socketutil.h
------------>>>> helper utility file for simplifying common socket operations in C.
Header guard. Prevents the file from being included multiple times during compilation.

stdio.h — for input/output (e.g., printf)
sys/socket.h, netinet/in.h, arpa/inet.h — all needed for socket creation and address manipulation
string.h — for memset, strcpy, etc.
malloc.h — for malloc

* struct sockaddr_in* createIPv4Address(char *ip, int port);
declaring a function that allocates and returns a pointer to a filled-in sockaddr_in structure using ip and port

int createTCPIpv4Socket();- creates a basic TCP (IPv4) socket 


### socketutil.c
------------>>>> implements the functions declared in your socketutil.h header, and it’s meant to simplify basic socket operations

AF_INET → IPv4
SOCK_STREAM → TCP
0 → Default protocol

Dynamically allocates a sockaddr_in struct (used in bind, connect, etc.).
Sets:
IPv4 family
Port (converted to network byte order)
IP address:
If ip is empty (""), uses INADDR_ANY (i.e., 0.0.0.0 → bind to all interfaces).
Else, converts the string IP to binary using inet_pton().


the problems in previous code were:
. Memory Leak Risk
malloc-ing a struct and returning a pointer, but nowhere in the code is this pointer ever freed. That’s a memory leak.
Better: Either return by value or clearly document that the caller must free() it.
. Poor Error Handling
malloc() succeeded (could return NULL)
inet_pton() succeeded (returns 1 on success)

### CMakeLists.txt 
is a build script for CMake that defines how to compile a small C library called SocketUtil

cmake_minimum_required(VERSION 3.23)
This sets the minimum required version of CMake to run this build script.

project(SocketUtil C)
SocketUtil is the name of the project.

set(CMAKE_C_STANDARD 99)
Tells the CMake compiler to use the C99 standard.
Enables things like // single-line comments, inline, designated initializers, etc.

add_library(SocketUtil socketutil.c)
key line. - ells CMake to build a static library called libSocketUtil.a from the source file socketutil.c.

After running cmake and then make, this will create:
libSocketUtil.a  # A static library you can link to in other projects

When and why you'd write this
modularizing code into a reusable library
Compile socketutil.c once
Link it in multiple projects without recompiling
Cleanly separate logic (e.g., main server logic vs low-level socket setup)

Is a Shared Library better than a Static Library?
That depends on your goals:
🔹 Static Library (.a)
Built into the final executable at compile time.
Pros:
No external dependencies — everything is self-contained.
Safer for deployment (easier to copy just one binary).
Cons:
Larger final executable.
If you update the library, you have to rebuild every program using it.
🔹 Shared Library (.so on Linux)
Linked at runtime, not baked into the executable.
Pros:
Smaller executable.
Multiple programs can share the same .so file.
Update the library once, and every linked program benefits.
Cons:
Deployment is trickier — the .so must be present on the system.
Slightly slower startup (dynamic linking happens at runtime).
🧠 Conclusion:
If this is a small self-contained project, static is simpler.
If you're planning to reuse the code across multiple apps, shared can be cleaner — especially in larger systems or when maintaining libraries.


