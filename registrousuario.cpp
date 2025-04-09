#include <iostream>
#include <fstream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <cstring>

std::string obtener_ip() {
    struct ifaddrs *addrs, *tmp;
    std::string ip = "127.0.0.1"; // ip definida por defecto
    
    getifaddrs(&addrs); // obtiene la lista de interfaces de red
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
    freeifaddrs(addrs); // libera la memoria
    return ip;
}

int main() {
    std::ifstream config("config.txt");
    int puerto = 8080;
    if(config.is_open()) {
        config >> puerto;
        config.close();
    }

    if (puerto < 1 || puerto > 65535) {
        std::cerr << "Error: Puerto inválido en config.txt\n";
        exit(EXIT_FAILURE);
    }

    bool salir = false;
    while (!salir) {
        std::cout << "\nSeleccione una opción:\n";
        std::cout << "1. Registrar usuario\n";
        std::cout << "2. Listar usuarios\n";
        std::cout << "3. Iniciar sesión\n";
        std::cout << "4. Salir\n";
        std::cout << "Opción: ";

        int opcion;
        std::cin >> opcion;

        if (opcion == 4) {
            std::cout << "Saliendo...\n";
            salir = true;
            break;
        }

        // 🧠 CADA VEZ CREAMOS EL SOCKET
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            std::cerr << "Error creando el socket\n";
            continue;
        }

        sockaddr_in servidor_addr;
        servidor_addr.sin_family = AF_INET;
        servidor_addr.sin_port = htons(puerto);
        inet_pton(AF_INET, "127.0.0.1", &servidor_addr.sin_addr);

        if (connect(sock, (struct sockaddr *)&servidor_addr, sizeof(servidor_addr)) < 0) {
            std::cerr << "Error conectando al servidor\n";
            close(sock);
            continue;
        }

        if (opcion == 1) {
            std::string usuario, contrasena;
            std::cout << "Nombre de usuario: ";
            std::cin >> usuario;
            std::cout << "Contraseña: ";
            std::cin >> contrasena;

            std::string registro = "REGISTRO|" + usuario + "|127.0.0.1|" + contrasena;
            send(sock, registro.c_str(), registro.size(), 0);

            char buffer[1024] = {0};
            read(sock, buffer, sizeof(buffer)-1);
            std::cout << "Respuesta del servidor: " << buffer << "\n";
        }
        else if (opcion == 2) {
            std::string comando = "LISTAR|";
            send(sock, comando.c_str(), comando.size(), 0);

            char buffer[1024] = {0};
            ssize_t bytes_leidos = read(sock, buffer, sizeof(buffer) - 1);

            if (bytes_leidos > 0) {
                buffer[bytes_leidos] = '\0'; 
                std::cout << "Usuarios registrados: " << buffer << "\n";
            } else {
                std::cerr << "Error recibiendo la lista de usuarios\n";
            }
        }
        else if (opcion == 3) {
            std::string usuario, contrasena;
            std::cout << "Nombre de usuario: ";
            std::cin >> usuario;
            std::cout << "Contraseña: ";
            std::cin >> contrasena;
        
            std::string login = "LOGIN|" + usuario + "|127.0.0.1|" + contrasena;
            send(sock, login.c_str(), login.size(), 0);
        
            char buffer[1024] = {0};
            read(sock, buffer, sizeof(buffer)-1);
            std::cout << "Respuesta del servidor: " << buffer << "\n";
        }
        close(sock); // 🔒 CERRAR SIEMPRE EL SOCKET DESPUÉS DE CADA OPERACIÓN
    }

    return 0;
}