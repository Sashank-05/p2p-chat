#include <stdio.h>
#include <winsock2.h>

#define PORT 8080
#define BUFFER_SIZE 512

DWORD WINAPI receiveMessages(LPVOID socket) {
    SOCKET client_fd = *((SOCKET *)socket);
    char buffer[BUFFER_SIZE] = {0};

    while (1) {
        if (recv(client_fd, buffer, BUFFER_SIZE, 0) <= 0) {
            printf("Receive failed\n");
            break;
        }
        printf(buffer);
        memset(buffer, 0, BUFFER_SIZE);
    }

    return 0;
}

DWORD WINAPI sendMessages(LPVOID socket) {
    SOCKET client_fd = *((SOCKET *)socket);
    char buffer[BUFFER_SIZE] = {0};

    while (1) {
        printf("Server: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        if (send(client_fd, buffer, strlen(buffer), 0) == SOCKET_ERROR) {
            printf("Send failed\n");
            break;
        }
        memset(buffer, 0, BUFFER_SIZE);
    }

    return 0;
}

int main() {
    WSADATA wsa;
    SOCKET server_fd, client_fd;
    struct sockaddr_in server, client;
    int addrlen = sizeof(struct sockaddr_in);
    HANDLE recv_thread, send_thread;
    DWORD recv_thread_id, send_thread_id;

    
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSA Startup failed\n");
        return 1;
    }
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);


    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Binding failed\n");
        return 1;
    }


    if (listen(server_fd, 3) == SOCKET_ERROR) {
        printf("Listen failed\n");
        return 1;
    }

    printf("listening on port %d\n", PORT);
    
    
    if ((client_fd = accept(server_fd, (struct sockaddr *)&client, &addrlen)) == INVALID_SOCKET) {
        printf("Acceptance failed\n");
        return 1;
    }

    printf("Client connected\n");

    
    recv_thread = CreateThread(NULL, 0, receiveMessages, &client_fd, 0, &recv_thread_id);
    if (recv_thread == NULL) {
        printf("Receive thread creation failed\n");
        return 1;
    }

    send_thread = CreateThread(NULL, 0, sendMessages, &client_fd, 0, &send_thread_id);
    if (send_thread == NULL) {
        printf("Send thread creation failed\n");
        return 1;
    }


    WaitForSingleObject(recv_thread, INFINITE);
    WaitForSingleObject(send_thread, INFINITE);


    closesocket(server_fd);
    closesocket(client_fd);
    WSACleanup();

    return 0;
}
