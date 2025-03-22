#include <iostream>
#include <map>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

std::map<std::string, std::pair<std::string, int>> usuarios; // Usuario: <IP, Puerto>

void manejar_registro(int cliente_sock) {
    char buffer[1024] = {0};
    read(cliente_sock, buffer, 1024);
    
    std::string datos(buffer);
    size_t pos1 = datos.find("|");
    size_t pos2 = datos.find("|", pos1+1);
    size_t pos3 = datos.find("|", pos2+1);
    
    std::string comando = datos.substr(0, pos1);
    std::string usuario = datos.substr(pos1+1, pos2-pos1-1);
    std::string ip = datos.substr(pos2+1, pos3-pos2-1);
    int puerto = stoi(datos.substr(pos3+1));
    
    if(comando == "REGISTRO") {
        usuarios[usuario] = {ip, puerto};
        std::cout << "Usuario registrado: " << usuario 
                  << " IP: " << ip 
                  << " Puerto: " << puerto << "\n";
    }
    std::string respuesta = "REGISTRO_OK|" + usuario;
    send(cliente_sock, respuesta.c_str(), respuesta.size(), 0);
    
    close(cliente_sock);
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);
    
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 5);
    
    while(true) {
        int cliente_sock = accept(server_fd, nullptr, nullptr);
        if(fork() == 0) { 
            manejar_registro(cliente_sock);
            exit(0);
        }
    }
    
    return 0;
}