#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "recibirmensaje.h"

void recibir_mensaje(int sock) {
    char buffer[1024];
    while(true) {
        int bytes_recibidos = recv(sock, buffer, sizeof(buffer), 0);
        if(bytes_recibidos > 0) {
            std::cout << "\033[1;37mMensaje recibido: " << std::string(buffer, bytes_recibidos) << "\033[0m\n";
        }
    }
}