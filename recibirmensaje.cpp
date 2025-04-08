#include <iostream> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

void recibir_mensaje(int puerto) {
    // Crear un socket para TCP
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Error al crear el socket de recepción.\n";
        return;
    }

    sockaddr_in direccion_cliente;
    direccion_cliente.sin_family = AF_INET;
    direccion_cliente.sin_addr.s_addr = INADDR_ANY;
    direccion_cliente.sin_port = htons(puerto);

    // Vincular el socket al puerto
    if (bind(sock, (struct sockaddr *)&direccion_cliente, sizeof(direccion_cliente)) < 0) {
        std::cerr << "Error al vincular el socket de recepción al puerto " << puerto << ".\n";
        close(sock);
        return;
    }

    // Escuchar conexiones entrantes
    if (listen(sock, 5) < 0) {
        std::cerr << "Error al escuchar en el puerto " << puerto << ".\n";
        close(sock);
        return;
    }

    std::cout << "Esperando conexión...\n";

    // Aceptar conexiones de clientes
    int cliente_sock = accept(sock, nullptr, nullptr);
    if (cliente_sock < 0) {
        std::cerr << "Error al aceptar la conexión.\n";
        close(sock);
        return;
    }

    std::cout << "Conexión aceptada. Esperando mensajes...\n";

    char buffer[1024];
    while (true) {
        ssize_t bytes_recibidos = recv(cliente_sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_recibidos < 0) {
            std::cerr << "Error al recibir mensaje.\n";
            break;
        }

        if (bytes_recibidos == 0) {
            std::cout << "Conexión cerrada por el cliente.\n";
            break;  // La conexión se ha cerrado
        }

        buffer[bytes_recibidos] = '\0';  // Aseguramos que el mensaje se termine en null
        std::cout << "Mensaje recibido: " << buffer << std::endl;
    }

    // Cerrar los sockets correctamente
    close(cliente_sock);
    close(sock);
}