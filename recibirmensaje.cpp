#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

void recibir_mensaje(int puerto) {
    int servidor_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (servidor_socket < 0) {
        std::cerr << "Error al crear el socket.\n";
        return;
    }

    sockaddr_in servidor_addr{};
    servidor_addr.sin_family = AF_INET;
    servidor_addr.sin_addr.s_addr = INADDR_ANY;
    servidor_addr.sin_port = htons(puerto);

    if (bind(servidor_socket, (struct sockaddr*)&servidor_addr, sizeof(servidor_addr)) < 0) {
        std::cerr << "Error al enlazar el socket al puerto.\n";
        close(servidor_socket);
        return;
    }

    if (listen(servidor_socket, 5) < 0) {
        std::cerr << "Error al poner el socket en modo escucha.\n";
        close(servidor_socket);
        return;
    }

    std::cout << "Esperando conexiones entrantes en el puerto " << puerto << "...\n";

    while (true) {
        sockaddr_in cliente_addr{};
        socklen_t cliente_len = sizeof(cliente_addr);

        int cliente_socket = accept(servidor_socket, (struct sockaddr*)&cliente_addr, &cliente_len);
        if (cliente_socket < 0) {
            std::cerr << "Error al aceptar la conexión.\n";
            continue;
        }

        std::cout << "Conexión aceptada. Esperando mensajes...\n";

        char buffer[1024];
        ssize_t bytes_recibidos;

        while ((bytes_recibidos = recv(cliente_socket, buffer, sizeof(buffer) - 1, 0)) > 0) {
            buffer[bytes_recibidos] = '\0';
            std::string mensaje(buffer);
            std::cout << "Mensaje recibido: " << mensaje << std::endl;
        }

        std::cout << "Conexión cerrada por el cliente.\n";
        close(cliente_socket);
    }

    close(servidor_socket); 
}
