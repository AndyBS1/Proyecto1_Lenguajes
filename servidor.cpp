#include <iostream>
#include <map>
#include <fstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <signal.h>

struct Usuario {
    std::string ip;
    int puerto;
    std::string contrasena;
};

std::map<std::string, Usuario> usuarios;
const std::string ARCHIVO_USUARIOS = "usuarios.txt";

void cargar_usuarios() {
    std::ifstream archivo(ARCHIVO_USUARIOS);
    if (!archivo.is_open()) return;
    
    std::string usuario, ip, contrasena;
    int puerto;
    while (archivo >> usuario >> ip >> puerto >> contrasena) {
        usuarios[usuario] = {ip, puerto, contrasena};
    }
    archivo.close();
}

void guardar_usuarios() {
    std::ofstream archivo(ARCHIVO_USUARIOS);
    for (const auto& [usuario, datos] : usuarios) {
        archivo << usuario << " " << datos.ip << " " << datos.puerto << " " << datos.contrasena << "\n";
    }
    archivo.close();
}

void manejar_registro(int cliente_sock) {
    char buffer[1024] = {0};
    ssize_t bytes_leidos = read(cliente_sock, buffer, sizeof(buffer)-1);
    if (bytes_leidos <= 0) {
        std::cerr << "Error leyendo datos o conexión cerrada\n";
        close(cliente_sock);
        return;
    }
    buffer[bytes_leidos] = '\0';

    std::string datos(buffer);
    std::cout << "Mensaje recibido: " << datos << std::endl;  // Para depuración

    // Caso especial para LISTAR
    if (datos == "LISTAR|") {
        std::string respuesta = "USUARIOS";
        if (usuarios.empty()) {
            respuesta += "|No hay usuarios registrados";
        } else {
            for (const auto& [usuario, datos] : usuarios) {
                respuesta += "|" + usuario;
            }
        }
        respuesta += "\n"; // Asegurar que el mensaje termina correctamente
        send(cliente_sock, respuesta.c_str(), respuesta.size(), 0);
        close(cliente_sock);
        return;
    }

    // Para REGISTRO
    size_t pos1 = datos.find("|");
    size_t pos2 = datos.find("|", pos1+1);
    size_t pos3 = datos.find("|", pos2+1);
    size_t pos4 = datos.find("|", pos3+1);

    if (pos1 == std::string::npos || pos2 == std::string::npos || pos3 == std::string::npos || pos4 == std::string::npos) {
        std::cerr << "Error: Formato de mensaje inválido\n";
        close(cliente_sock);
        return;
    }    

    std::string comando = datos.substr(0, pos1);
    std::string usuario = datos.substr(pos1+1, pos2-pos1-1);
    std::string ip = datos.substr(pos2+1, pos3-pos2-1);
    int puerto = std::stoi(datos.substr(pos3+1, pos4-pos3-1));
    std::string contrasena = datos.substr(pos4+1);

    if (comando == "REGISTRO") {
        if (usuarios.find(usuario) != usuarios.end()) {
            std::cerr << "Intento de registro duplicado: " << usuario << "\n";
            send(cliente_sock, "ERROR|Usuario ya existe", 23, 0);
        } else {
            usuarios[usuario] = {ip, puerto, contrasena};
            std::cout << "Usuario registrado: " << usuario << " IP: " << ip << " Puerto: " << puerto << "\n";
            send(cliente_sock, "OK|Registro exitoso", 19, 0);
            guardar_usuarios();
        }
    }

    close(cliente_sock);
}

int main() {
    cargar_usuarios();
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Error creando el socket: " << strerror(errno) << "\n";
        exit(EXIT_FAILURE);
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 5);
    signal(SIGCHLD, SIG_IGN);

    std::cout << "Servidor iniciado en puerto 8080\n";

    while(true) {
        int cliente_sock = accept(server_fd, nullptr, nullptr);
        if (cliente_sock >= 0) {
            manejar_registro(cliente_sock);
        }
    }

    close(server_fd);
    return 0;
}