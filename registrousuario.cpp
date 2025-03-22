#include <iostream>
#include <fstream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <netinet/in.h>

std::string obtener_ip() {
    struct ifaddrs *addrs, *tmp;
    std::string ip = "127.0.0.1"; // Default a localhost si no encuentra otra
    
    getifaddrs(&addrs);
    tmp = addrs;

    while (tmp) {
        if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *pAddr = (struct sockaddr_in *)tmp->ifa_addr;
            if(std::string(tmp->ifa_name) != "lo") { // Ignorar loopback
                ip = inet_ntoa(pAddr->sin_addr);
                break;
            }
        }
        tmp = tmp->ifa_next;
    }
    freeifaddrs(addrs);
    return ip;
}

int main() {
    std::ifstream config("config.txt");
    int puerto = 8080; 
    if(config.is_open()) {
        config >> puerto;
        config.close();
    }

    // Crear socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in servidor_addr;
    servidor_addr.sin_family = AF_INET;
    servidor_addr.sin_port = htons(puerto);
    inet_pton(AF_INET, "127.0.0.1", &servidor_addr.sin_addr); 

    connect(sock, (struct sockaddr *)&servidor_addr, sizeof(servidor_addr));

    std::string usuario;
    std::cout << "Nombre de usuario: ";
    std::cin >> usuario;
    
    std::string ip = obtener_ip();
    
    std::string registro = "REGISTRO|" + usuario + "|" + ip + "|" + std::to_string(puerto);
    send(sock, registro.c_str(), registro.size(), 0);

    char buffer[1024] = {0};
    int bytes_recibidos = recv(sock, buffer, sizeof(buffer), 0);

    if(bytes_recibidos > 0) {
        std::cout << "\033[1;32mRespuesta del servidor: " 
              << std::string(buffer, bytes_recibidos) << "\033[0m\n";
    } else {
    std::cerr << "\033[1;31mError en la comunicación\033[0m\n";
    }
    
    close(sock);
    return 0;
}