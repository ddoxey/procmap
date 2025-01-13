#include <iostream>
#include <thread>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// Define default ports if not set via compiler
#ifndef LISTEN_PORT
#define LISTEN_PORT 6600
#endif

#ifndef CONNECT_PORT
#define CONNECT_PORT 6599
#endif

void log_message(const std::string &message) {
    std::cout << "[" << getpid() << "] Received: " << message << std::endl;
}

void server(int port) {
    int server_fd, client_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind to the specified port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "[" << getpid() << "] Listening on port " << port << std::endl;

    while (true) {
        if ((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            sleep(1);
            continue;
        }

        char buffer[1024] = {0};
        int bytes_read = read(client_fd, buffer, sizeof(buffer));
        if (bytes_read > 0) {
            log_message(std::string(buffer, bytes_read));
        }
    }

    close(client_fd);
}

void client(int port) {
    int sock = 0;
    struct sockaddr_in serv_addr;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return;
    }

    while (true) {
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("Socket creation error");
            sleep(2); // Wait before retrying
            continue;
        }

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == 0) {
            std::cout << "[" << getpid() << "] Connected to server on port " << port << std::endl;
            break;
        } else {
            perror("Connection failed");
            close(sock); // Close the socket before retrying
            sleep(2); // Wait before retrying
        }
    }

    while (true) {
        std::string message = "hello " + std::to_string(getpid());
        if (send(sock, message.c_str(), message.size(), 0) < 0) {
            perror("Send failed");
            break;
        }
        std::cout << "[" << getpid() << "] Sent: " << message << std::endl;
        sleep(2); // Send a message every 2 seconds
    }

    close(sock); // Close the connection when done
}

int main() {
    std::thread server_thread(server, LISTEN_PORT);
    std::thread client_thread(client, CONNECT_PORT);

    server_thread.join();
    client_thread.join();

    return 0;
}
