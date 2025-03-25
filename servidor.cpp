#include <iostream>
#include <map>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

std::map<std::string, std::pair<std::string, int>> usuarios; // Usuario: <IP, Puerto>

void manejar_cliente(int cliente_sock) {
    char buffer[1024] = {0};
    int bytes_recibidos = read(cliente_sock, buffer, sizeof(buffer));

    if (bytes_recibidos > 0) {
        std::string datos(buffer);
        size_t pos1 = datos.find("|");
        size_t pos2 = datos.find("|", pos1 + 1);
        size_t pos3 = datos.find("|", pos2 + 1);

        std::string comando = datos.substr(0, pos1);

        if (comando == "REGISTRO") {
            std::string usuario = datos.substr(pos1 + 1, pos2 - pos1 - 1);
            std::string ip = datos.substr(pos2 + 1, pos3 - pos2 - 1);
            int puerto = std::stoi(datos.substr(pos3 + 1));

            usuarios[usuario] = {ip, puerto};
            std::cout << "[REGISTRO] Usuario: " << usuario << " | IP: " << ip << " | Puerto: " << puerto << "\n";

            std::string respuesta = "REGISTRO_OK|" + usuario;
            send(cliente_sock, respuesta.c_str(), respuesta.size(), 0);
        } 
        else if (comando == "MENSAJE") {
            // Datos que se reciben en el server
            std::string usuario_origen = datos.substr(pos1 + 1, pos2 - pos1 - 1);
            std::string usuario_destino = datos.substr(pos2 + 1, pos3 - pos2 - 1);
            std::string mensaje = datos.substr(pos3 + 1);
            
            // Mensaje recibido
            std::cout << "[MENSAJE] " 
                      << "De: " << usuario_origen << " , "
                      << "Para: " << usuario_destino << " -> "
                      << mensaje << "\n";
        }
    }

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

    std::cout << "Servidor esperando conexiones en el puerto 8080...\n";

    while (true) {
        int cliente_sock = accept(server_fd, nullptr, nullptr);
        if (cliente_sock >= 0) {
            if (fork() == 0) { 
                manejar_cliente(cliente_sock);
                exit(0);
            }
        }
    }

    return 0;
}