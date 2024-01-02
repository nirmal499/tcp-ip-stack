#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "defined_type.h"

const int SERVER_PORT = 12345;
const int BUFFER_SIZE = 2048;

int main() {
    // Create socket
    int clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }

    // Server address
    sockaddr_in serverAddress;
    std::memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

    // Send a message to the server
    // const char* message = "Hello from UDP client!";
    // sendto(clientSocket, message, std::strlen(message), 0, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    
    char buffer[BUFFER_SIZE];

    std::memset(buffer, 0, sizeof(buffer));

    Defined defined_data;
    defined_data.initValue1();
    std::size_t encoded_size = defined_data.encode_into_buffer(buffer, sizeof(buffer));
    if(encoded_size == 0){
        std::cerr << "Error encoding data" << std::endl;
        return -1;
    }

    std::cout << "Sending data to server: \n";
    print_data(&defined_data);
    
    sendto(clientSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    // Receive response from the server
    // char buffer[BUFFER_SIZE];

    sockaddr_in serverResponseAddress;
    socklen_t serverResponseAddressLength = sizeof(serverResponseAddress);

    std::memset(buffer, 0, sizeof(buffer));
    ssize_t receivedBytes = recvfrom(clientSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&serverResponseAddress, &serverResponseAddressLength);

    if (receivedBytes == -1) {
        std::cerr << "Error receiving response" << std::endl;
        return -1;
    }

    std::size_t decoded_size = defined_data.decode_from_buffer(buffer);
    if(decoded_size == 0){
        std::cerr << "Error decoding data" << std::endl;
        return -1;
    }

    std::cout << "Received response from server: \n";
    print_data(&defined_data);
    
    // if (receivedBytes == -1) {
    //     std::cerr << "Error receiving response" << std::endl;
    // } else {
    //     buffer[receivedBytes] = '\0'; // Null-terminate the received data
    //     std::cout << "Received response from server: " << buffer << std::endl;
    // }

    // Close the socket
    close(clientSocket);

    return 0;
}
