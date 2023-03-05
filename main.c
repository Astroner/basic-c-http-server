#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

int main(void) {

    // Create a socket
    int socket_fd = socket(
        AF_INET, // (address family) IP addresses
        SOCK_STREAM, // (type) connection based protocol
        0 // (protocol) specifying the protocol of communication for pair SOCK_STREAM/AF_INET
    );

    if(socket_fd < 0) {
        printf("Cannot create socket\n");

        return 1;
    }

    // Create an address for socket
    struct sockaddr_in addr = {
        .sin_family = AF_INET, // (address family) same as used in socket
        .sin_addr = { // ("host")
            // htonl() just reverses a number. network uses this format
            .s_addr = htonl(INADDR_ANY), // INADDR_ANY stands for "0.0.0.0" 
        },
        .sin_port = htons(4040), // (port) htons = htonl but for short
    };


    // bind address to socket 
    if(bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < -1) {
        printf("Failed to bind socket\n");

        return 1;
    }

    if(
        listen(// sys call that marks socket as listener
            socket_fd, // (socket)
            3 // (backlog) connections queue length
        ) < 0
    ) {
        printf("Failed to start server\n");

        return 1;
    }

    // Basically, socket we've created only suitable for accepting connections, not data exchanging
    // So we need another one 
    int incomingSocket;
    struct sockaddr_in incomingSocketAddress;
    socklen_t incomingSocketAddressLength;

    // listening loop
    while(1) {
        printf("# Waiting for requests\n");
        if(
            (
                incomingSocket = accept( // get first connection request from the queue
                    socket_fd, // (socket)
                    (struct sockaddr*)&incomingSocketAddress, // (client address) 
                    &incomingSocketAddressLength // (address struct size)
                )
            ) < 0
        ) {
            printf("# Failed to accept the connection\n");

            return 1;
        };

        printf(
            "## New Request from %s\n", 
            inet_ntoa(incomingSocketAddress.sin_addr) // inet_ntoa() converts addr struct to human-readable IP
        );
        char buffer[3000];
    
        if(read(incomingSocket, buffer, 3000) < 0) {
            printf("### Nothing to read\n");
        }

        printf("### Incoming Message:\n\n");
        printf("%s\n", buffer);
        printf("### End of message\n");

        // Minimal required headers for HTTP response are HTTP version, Content-Type and Content-Length
        char* hello = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 77\n\n<html><head><title>Hi mom!</title></head><body><h1>Hi Mom!</h1></body></html>";

        write(incomingSocket, hello, strlen(hello));
        printf("### 'Hi' is sent\n\n");

        close(incomingSocket);
    }

    return 0;
}