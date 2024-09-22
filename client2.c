#include <windows.h>
#include <WinSock2.h>
#include <stdio.h>

#define port 8000
#define buffersize 512


DWORD WINAPI receiveMessages(LPVOID socket) {
    SOCKET sock = *((SOCKET *)socket);
    char buffer[buffersize] = {0};

    while (1) {
        if (recv(sock, buffer, buffersize, 0) <= 0) {
            printf("Receive failed\n");
            break;
        }
        memset(buffer, 0, buffersize);
    }

    return 0;
}

DWORD WINAPI sendMessages(LPVOID socket) {
    SOCKET sock = *((SOCKET *)socket);
    char buffer[buffersize] = {0};

    while (1) {
        printf("Client: ");
        fgets(buffer, buffersize, stdin);
        if (send(sock, buffer, strlen(buffer), 0) == SOCKET_ERROR) {
            printf("Send failed\n");
            break;
        }
        memset(buffer, 0, buffersize);
    }

    return 0;
}

int main(){
    WSADATA wsa;
    struct sockaddr_in server;
    SOCKET sock;
    HANDLE senderT, recieverT;
    DWORD senderTID, recieverTID;


    if(WSAStartup(MAKEWORD(2,2),&wsa) != 0){
        printf("WSA Startup Failed!\n");
    }
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        return 1;
    }


    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Connection failed\n");
        return 1;
    }

    printf("Connected to server\n");

    recieverT = CreateThread(NULL, 0, receiveMessages, &sock, 0, &recieverTID);
    if (recieverT == NULL) {
        printf("Receive thread creation failed\n");
        return 1;
    }

    senderT = CreateThread(NULL, 0, sendMessages, &sock, 0, &senderTID);
    if (senderT == NULL) {
        printf("Send thread creation failed\n");
        return 1;
    }

    WaitForSingleObject(senderT, INFINITE);
    WaitForSingleObject(recieverT, INFINITE);

    closesocket(sock);
    WSACleanup();

    return 0;

}