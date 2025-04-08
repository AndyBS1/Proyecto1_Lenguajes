#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h>
#include "enviarmensaje.h"
#include "recibirmensaje.h"

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

    // Usamos fork() para crear dos procesos
    pid_t pid = fork(); // pid --> Process ID

    if (pid == -1) {
        std::cerr << "Error al crear el proceso hijo.\n";
        return 1;
    }

    if (pid == 0) {  // Proceso hijo: recibe los mensajes
        recibir_mensaje(sock);  
    } else {  // Proceso padre: enviar mensaje
        enviar_mensaje(usuario_origen, usuario_destino, mensaje); 
        wait(NULL);  
    }

    close(sock);
    return 0;
}