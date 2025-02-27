# Battleship Socket Game / Amiral Battı Soket Oyunu

---

## English

### Overview
This is a client-server implementation of the classic Battleship game using C and the Winsock library. Two players can connect to a server and compete against each other by trying to find and sink each other's ships on a 5x5 grid.

### Game Rules
- Each player has a 5×5 board with 3 ships randomly placed
- Players take turns guessing coordinates to hit the opponent's ships
- The first player to score 3 hits (sink all enemy ships) wins the game
- Board symbols:
  - `-`: Empty water/unknown
  - `S`: Ship (visible only on your own board)
  - `X`: Hit ship
  - `O`: Missed shot

### Technical Details
- Client-server architecture using TCP sockets with Winsock
- Default server IP: 127.0.0.1 (localhost)
- Default port: 8080
- Written in C

### Project Structure
- `server.c`: Server implementation that handles game logic and player connections
- `client.c`: Client implementation for player interaction
- `server.exe`: Compiled server executable
- `client.exe`: Compiled client executable

### Requirements
- Windows operating system
- C compiler (GCC recommended)
- Winsock library (ws2_32.lib)

### How to Run
1. Start the server by running `server.exe`
2. Two clients can connect by running `client.exe` on separate terminals
3. Follow the on-screen instructions to play
4. Enter coordinates in the format of a letter followed by a number (e.g., B3)

### Building from Source
To compile the source code, you need a C compiler with Winsock library support:

```bash
gcc server.c -o server.exe -lws2_32
gcc client.c -o client.exe -lws2_32
```

### Troubleshooting
- **Connection Failed**: Ensure the server is running before starting clients
- **Address Already in Use**: Close any previous instances of the server or change the port
- **Invalid Coordinates**: Enter coordinates in the format A1-E5 (letter followed by number)

### Future Enhancements
- Support for different board sizes
- Customizable number of ships
- Ships with different sizes
- Graphical user interface
- Online multiplayer support

---

## Türkçe

### Genel Bakış
Bu, klasik Amiral Battı oyununun C ve Winsock kütüphanesi kullanılarak yapılmış istemci-sunucu uygulamasıdır. İki oyuncu bir sunucuya bağlanabilir ve 5x5 ızgara üzerinde birbirlerinin gemilerini bulmaya çalışarak yarışabilir.

### Oyun Kuralları
- Her oyuncunun 5×5 tahtası üzerinde rastgele yerleştirilmiş 3 gemisi vardır
- Oyuncular sırayla rakiplerinin gemilerini vurmak için koordinat tahminleri yapar
- İlk olarak 3 vuruş yapan (tüm düşman gemilerini batıran) oyuncu kazanır
- Tahta sembolleri:
  - `-`: Boş su/bilinmeyen
  - `S`: Gemi (yalnızca kendi tahtanızda görünür)
  - `X`: Vurulan gemi
  - `O`: Kaçırılan atış

### Teknik Detaylar
- Winsock ile TCP soket kullanan istemci-sunucu mimarisi
- Varsayılan sunucu IP: 127.0.0.1 (localhost)
- Varsayılan port: 8080
- C dilinde yazılmıştır

### Proje Yapısı
- `server.c`: Oyun mantığını ve oyuncu bağlantılarını yöneten sunucu uygulaması
- `client.c`: Oyuncu etkileşimi için istemci uygulaması
- `server.exe`: Derlenmiş sunucu çalıştırılabilir dosyası
- `client.exe`: Derlenmiş istemci çalıştırılabilir dosyası

### Gereksinimler
- Windows işletim sistemi
- C derleyicisi (GCC önerilir)
- Winsock kütüphanesi (ws2_32.lib)

### Nasıl Çalıştırılır
1. `server.exe` dosyasını çalıştırarak sunucuyu başlatın
2. İki istemci ayrı terminallerde `client.exe` dosyasını çalıştırarak bağlanabilir
3. Oynamak için ekrandaki talimatları izleyin
4. Koordinatları harf ve rakam şeklinde girin (örneğin, B3)

### Kaynak Koddan Derleme
Kaynak kodu derlemek için Winsock kütüphanesi desteği olan bir C derleyicisine ihtiyacınız var:

```bash
gcc server.c -o server.exe -lws2_32
gcc client.c -o client.exe -lws2_32
```
