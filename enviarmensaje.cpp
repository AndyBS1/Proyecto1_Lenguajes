#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    std::string usuario_origen;
    std::cout << "De: ";
    std::cin >> usuario_origen;
    std::cin.ignore();

    std::string usuario_destino;
    std::cout << "Para: ";
    std::cin >> usuario_destino;
    std::cin.ignore(); // Se limpia el buffer
    
    std::string mensaje;
    std::cout << "Mensaje: ";
    std::getline(std::cin, mensaje);
    
    // Configuración de la conexión al servidor
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in servidor_address;
    servidor_address.sin_family = AF_INET;
    servidor_address.sin_port = htons(8080); // Puerto del server: 8080
    inet_pton(AF_INET, "127.0.0.1", &servidor_address.sin_addr);
    
    if (connect(sock, (struct sockaddr *)&servidor_address, sizeof(servidor_address)) < 0) {
        std::cerr << "Error al conectar con el servidor.\n";
        return 1;
    }
    
    // Formato del mensaje
    std::string mensaje_formato = "MENSAJE|" + usuario_origen + "|" + usuario_destino + "|" + mensaje;
    
    // Envio del mensaje al server
    send(sock, mensaje_formato.c_str(), mensaje_formato.size(), 0);
    
    std::cout << "Mensaje enviado al servidor.\n";
    
    close(sock);
    return 0;
}