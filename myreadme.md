First of all in Berkeley Sockets
we would create our own socket and currently we are writing the client side of the application
so as are we writing a client there must. be a server sittling idle waiting to listen for incoming connection
we will write the server side later on
port no will be the number that the application will acquire form the OS
so that other side could connect to it
ports from 0 to 1000 are reserved

# TCP = Transmission Control Protocol
reliable, connection-based protocol built on top of IP (Internet Protocol).
Key Features of TCP:
1.Guarantees delivery of packets in order
2.Detects lost or corrupted packets and retransmits
3Provides a stream-oriented connection (like a pipe between two systems)
4.Handles flow control, congestion control, and error checking

# Why TCP in chat apps?
You need messages to arrive in full and in order
You want the connection to stay alive (like a phone call)
Think of TCP as a virtual wire between two systems — you write on one end, it comes out the other, no mess.

# POSIX Threads (pthreads)?  POSIX = Portable Operating System Interface
 standard threading library on UNIX-based systems (Linux, macOS)
 Let’s you run multiple tasks in parallel within the same process
 a family of standards for UNIX systems — including threads.

# How POSIX threads are used in your code?
pthread_t id ;
pthread_create(&id, NULL, listenAndPrint, socketFD);

* listenAndPrint= Function that this new thread will run
* socketFD = Passed as an argument (shared resource)
So now:
main() continues with readConsoleEntriesAndSendToServer()
While in parallel, listenAndPrint() runs in a background thread.

# SOCKETclient -main.c code explanation
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
# multi-client TCP chat server.

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