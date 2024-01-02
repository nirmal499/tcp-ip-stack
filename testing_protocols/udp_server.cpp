#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "defined_type.h"

const int PORT = 12345;
const int BUFFER_SIZE = 2048;

int main() {
    // Create socket
    int serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }

    // Bind the socket
    sockaddr_in serverAddress;
    std::memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error binding socket" << std::endl;
        close(serverSocket);
        return -1;
    }

    std::cout << "UDP server is listening on port " << PORT << std::endl;

    while (true) {
        char buffer[BUFFER_SIZE];
        sockaddr_in clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);

        std::memset(buffer, 0, sizeof(buffer));

        // Receive data from the client
        ssize_t receivedBytes = recvfrom(serverSocket, buffer, sizeof(buffer), 0,
                                        (struct sockaddr*)&clientAddress, &clientAddressLength);

        if (receivedBytes == -1) {
            std::cerr << "Error receiving data" << std::endl;
            break;
        }

        Defined defined_data;
        std::size_t decoded_size = defined_data.decode_from_buffer(buffer);

        if(decoded_size == 0){
            std::cerr << "Error decoding data" << std::endl;
            break;
        }

        print_data(&defined_data);
        std::cout << "Received from " << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << std::endl;

        // Print received data
        // buffer[receivedBytes] = '\0'; // Null-terminate the received data
        // std::cout << "Received from " << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << " - " << buffer << std::endl;
        

        // Send a response back to the client
        // const char* responseMessage = "Hello from UDP server!";
        // sendto(serverSocket, responseMessage, std::strlen(responseMessage), 0, (struct sockaddr*)&clientAddress, sizeof(clientAddress));

        sendto(serverSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientAddress, sizeof(clientAddress));

    }

    // Close the socket
    close(serverSocket);

    return 0;
}
