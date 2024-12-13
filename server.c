#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024
#define BOARD_SIZE 5
#define WINNING_SCORE 3

char player_boards[2][BOARD_SIZE][BOARD_SIZE]; // Oyuncuların tahtaları
char player_hits[2][BOARD_SIZE][BOARD_SIZE];  // Oyuncuların rakip tahtası üzerinde yaptıkları hamleleri gösterecek tahtalar
int player_scores[2] = {0, 0}; // Oyuncuların skorları

// Tahtayı başlatmak random için srandd unutma !!!!!
void initialize_board(char board[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = '-';
        }
    }

    int gems_placed = 0;
    while (gems_placed < 3) {
        int x = rand() % BOARD_SIZE;
        int y = rand() % BOARD_SIZE;

        if (board[x][y] != 'S') {
            board[x][y] = 'S'; // Gemiler 
            gems_placed++;
        }
    }
}

// Tahtayı metin formatında oluşturmak
void format_board(char board[BOARD_SIZE][BOARD_SIZE], char *output, size_t output_size, int hidden) {
    snprintf(output, output_size, "   1  2  3  4  5\n");
    size_t current_len = strlen(output);
    
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (current_len < output_size) {
            snprintf(output + current_len, output_size - current_len, "%c ", 'A' + i);
            current_len = strlen(output);
            
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (current_len >= output_size) break;
                char cell = (hidden && board[i][j] == 'S') ? '-' : board[i][j];
                snprintf(output + current_len, output_size - current_len, " %c ", cell);
                current_len = strlen(output);
            }
            
            if (current_len < output_size) {
                snprintf(output + current_len, output_size - current_len, "\n");
                current_len = strlen(output);
            }
        }
    }
}

// Yapılan hamleyi isleyip sonucu döndürmek 
char process_move(char board[BOARD_SIZE][BOARD_SIZE], int x, int y, int *score) {
    if (board[x][y] == 'S') {
        board[x][y] = 'X';
        (*score)++;
        return 'H'; // vurdu
    } else if (board[x][y] == '-') {
        board[x][y] = 'O';
        return 'M'; // kaçırmak
    }
    return 'A'; // önceden vurulmuş
}

// Tahtayı kontrol et
int check_game_over() {
    for (int i = 0; i < 2; i++) {
        if (player_scores[i] >= WINNING_SCORE) {
            return i + 1; // Kazanan oyuncunun indexi (1 tabanlı) 1-2
        }
    }
    return 0; // oyun bitmedi
}

// Tahtaları oyunculara gönder !!!!
void send_boards_to_players(SOCKET client_sockets[2]) {
    char buffer[BUFFER_SIZE];

    for (int i = 0; i < 2; i++) {
        memset(buffer, 0, BUFFER_SIZE);
        format_board(player_boards[i], buffer, BUFFER_SIZE, 0); // Kendi tahtasında gemilerle birlikte
        send(client_sockets[i], buffer, strlen(buffer), 0);

        memset(buffer, 0, BUFFER_SIZE);
        format_board(player_hits[i], buffer, BUFFER_SIZE, 1); // Rakip tahtasında gemileri gizle
        send(client_sockets[i], buffer, strlen(buffer), 0);
    }
}

// Oyun durumunu gönderen yeni bir fonksiyon ekleyin
void send_game_status(SOCKET client_sockets[2], int current_player) {
    char status[BUFFER_SIZE];
    sprintf(status, "Skor - Oyuncu 1: %d, Oyuncu 2: %d\nSira: Oyuncu %d\n",
            player_scores[0], player_scores[1], current_player + 1);
    
    for (int i = 0; i < 2; i++) {
        send(client_sockets[i], status, strlen(status), 0);
    }
}

int send_with_check(SOCKET sock, const char *buffer, int len, int flags) {
    int result = send(sock, buffer, len, flags);
    if (result == SOCKET_ERROR) {
        printf("Send failed with error: %d\n", WSAGetLastError());
        return -1;
    }
    return result;
}

int recv_with_check(SOCKET sock, char *buffer, int len, int flags) {
    int result = recv(sock, buffer, len, flags);
    if (result == 0) {
        printf("Connection closed by peer\n");
        return 0;
    }
    if (result == SOCKET_ERROR) {
        printf("Recv failed with error: %d\n", WSAGetLastError());
        return -1;
    }
    return result;
}

void cleanup_sockets(SOCKET server_socket, SOCKET *client_sockets, int num_clients) {
    for (int i = 0; i < num_clients; i++) {
        if (client_sockets[i] != INVALID_SOCKET) {
            closesocket(client_sockets[i]);
        }
    }
    if (server_socket != INVALID_SOCKET) {
        closesocket(server_socket);
    }
    WSACleanup();
}

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_sockets[2];
    struct sockaddr_in server_addr, client_addr;
    int client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    int current_player = 0;
    int connected_players = 0;

    srand(time(NULL)); //random sayı 

    // Winsock başlat
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Winsock initialization failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    printf("Sunucu soketi basariyla olusturuldu.\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    printf("Baglanma basarili oyuncular bekleniyor...\n");

    listen(server_socket, 2);

    // İki oyuncuyu bağla
    while (connected_players < 2) {
        client_sockets[connected_players] = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        printf("Oyuncu %d baglandi.\n", connected_players + 1);
        connected_players++;
    }

    printf("Iki oyuncu da baglandi oyun basliyor...\n");

    // İki oyuncuya da tahtaları başlat
    for (int i = 0; i < 2; i++) {
        initialize_board(player_boards[i]);
        initialize_board(player_hits[i]);
    }

    // Her iki oyuncuya tahtalarını gönder
    send_boards_to_players(client_sockets);

    // Server socket için timeout
    DWORD timeout = 30000; // 30 saniye
    setsockopt(server_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    setsockopt(server_socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));

    // Oyun döngüsü
    while (1) {
        // Güncel tahtaları her iki oyuncuya gönder
        send_boards_to_players(client_sockets);

        // Oyun bitti mi kontrolü
        int winner = check_game_over();
        if (winner) {
            char winner_msg[] = "Tebrikler! Oyunu kazandiniz!\nOyun sona erdi.\n";
            char loser_msg[] = "Maalesef kaybettiniz.\nOyun sona erdi.\n";
            
            send(client_sockets[winner - 1], winner_msg, strlen(winner_msg), 0);
            send(client_sockets[2 - winner], loser_msg, strlen(loser_msg), 0);
            break;
        }

        // Oyunculara durum mesajı gönder
        char status[BUFFER_SIZE];
        sprintf(status, "Skor - Oyuncu 1: %d, Oyuncu 2: %d\n",
                player_scores[0], player_scores[1]);
        
        for (int i = 0; i < 2; i++) {
            send(client_sockets[i], status, strlen(status), 0);
            
            memset(buffer, 0, BUFFER_SIZE);
            if (i == current_player) {
                sprintf(buffer, "Sira sizde. Konum sec (Ornek: B3): ");
            } else {
                sprintf(buffer, "Sira Oyuncu %d'de. Lutfen bekleyin...\n", current_player + 1);
            }
            send(client_sockets[i], buffer, strlen(buffer), 0);
        }

        // Sıradaki oyuncudan hamle al
        memset(buffer, 0, BUFFER_SIZE);
        int recv_result = recv_with_check(client_sockets[current_player], buffer, BUFFER_SIZE, 0);
        if (recv_result <= 0) {
            printf("Baglanti hatasi\n");
            break;
        }

        // Giriş doğrulama
        if (strlen(buffer) < 2 || 
            buffer[0] < 'A' || buffer[0] >= 'A' + BOARD_SIZE ||
            buffer[1] < '1' || buffer[1] >= '1' + BOARD_SIZE) {
            char error_msg[] = "Gecersiz koordinat! Lutfen tekrar deneyin.\n";
            send(client_sockets[current_player], error_msg, strlen(error_msg), 0);
            continue;
        }

        int x = buffer[0] - 'A';
        int y = buffer[1] - '1';

        // Hamleyi işle
        char result = process_move(player_boards[1 - current_player], x, y, &player_scores[current_player]);
        
        // Hamle sonucunu kaydet
        if (result == 'H') {
            player_hits[current_player][x][y] = 'X';
            char hit_msg[] = "Tebrikler! Gemi vurdunuz!\n";
            send(client_sockets[current_player], hit_msg, strlen(hit_msg), 0);
        } else if (result == 'M') {
            player_hits[current_player][x][y] = 'O';
            char miss_msg[] = "Iskaladiniz!\n";
            send(client_sockets[current_player], miss_msg, strlen(miss_msg), 0);
        } else {
            char already_msg[] = "Bu koordinat daha once secildi!\n";
            send(client_sockets[current_player], already_msg, strlen(already_msg), 0);
            continue;
        }

        // Sırayı değiştir
        current_player = 1 - current_player;
    }

    // Bağlantıları kapat
    printf("Oyun bitti, sunucu kapatiliyor...\n");
    cleanup_sockets(server_socket, client_sockets, 2);

    return 0;
}
