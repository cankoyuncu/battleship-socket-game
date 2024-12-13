#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

// Tahtayı ekranda düzgün bir şekilde göster
void display_board(const char *title, const char *board) {
    printf("\n=== %s ===\n", title);
    printf("%s\n", board);
}

int main() {
    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Winsock'u başlat
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Winsock initialization failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    // Sunucuya bağlan
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Connection failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    printf("Sunucuya Baglanildi.\n");

    // Oyun döngüsü
    while (1) {
        int recv_result = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (recv_result <= 0) {
            printf("Server baglantisi kesildi.\n");
            break;
        }
        display_board("TAHTALAR (Sen-Rakip)", buffer);

        // Rakip tahtasını al
        memset(buffer, 0, BUFFER_SIZE);
        recv(client_socket, buffer, BUFFER_SIZE, 0);
        display_board("ATISLAR", buffer);

        // Giriş doğrulama ekleyin
        do {
            printf("Konum sec (Ornek: B3): ");
            fgets(buffer, BUFFER_SIZE, stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            
            if (strlen(buffer) != 2 || 
                buffer[0] < 'A' || buffer[0] >= 'A' + 5 ||
                buffer[1] < '1' || buffer[1] >= '1' + 5) {
                printf("Gecersiz koordinat! A1-E5 arasinda bir deger girin.\n");
                continue;
            }
            break;
        } while(1);

        send(client_socket, buffer, strlen(buffer), 0);

        memset(buffer, 0, BUFFER_SIZE);
        recv(client_socket, buffer, BUFFER_SIZE, 0);
        printf("\n%s\n", buffer);

        if (strstr(buffer, "Tebrikler kazandin!") || strstr(buffer, "Kaybettin!")) {
            break;
        }
    }
    
    closesocket(client_socket);
    WSACleanup();
    return 0;
}
