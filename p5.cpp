#include <thread>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/types.h>
#include<iostream>

using namespace std;

#define PORT 8080
#define BUFFER_SIZE 1024

void handleClient(int clientSocket) {
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead = read(clientSocket, buffer, BUFFER_SIZE - 1);
    
    if (bytesRead < 0) {
        cerr << "Failed to read from client" << endl;
        close(clientSocket);
        return;
    }

    buffer[bytesRead] = '\0';

    const char *httpHeader = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %zu\r\n\r\n";
    const char *htmlContent = "<html><body><h1>Welcome to server !</h1></body></html>";
    string response = string(httpHeader) + string(htmlContent);

    ssize_t bytesWritten = write(clientSocket, response.c_str(), response.size());
    
    if (bytesWritten < 0) {
        cerr << "Failed to write to client" << endl;
    }

    close(clientSocket);
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        cerr << "Socket creation failed" << endl;
        return EXIT_FAILURE;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Bind failed" << endl;
        close(serverSocket);
        return EXIT_FAILURE;
    }

    if (listen(serverSocket, 5) < 0) {
        cerr << "Listen failed" << endl;
        close(serverSocket);
        return EXIT_FAILURE;
    }

    cout << "Server is listening on port " << PORT << endl;

    while (true) {
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (clientSocket < 0) {
            cerr << "Client accept failed" << endl;
            continue;
        }

        thread clientThread(handleClient, clientSocket);
        clientThread.detach();
    }

    close(serverSocket);
    return 0;
}
