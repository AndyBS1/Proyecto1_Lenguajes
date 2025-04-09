#include "HiloReceptor.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

HiloReceptor::HiloReceptor(int puerto, QObject *parent)
    : QThread(parent), puertoEscucha(puerto) {}

void HiloReceptor::run() {
    int servidor_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (servidor_socket < 0) {
        std::cerr << "Error al crear el socket.\n";
        return;
    }

    sockaddr_in servidor_addr{};
    servidor_addr.sin_family = AF_INET;
    servidor_addr.sin_addr.s_addr = INADDR_ANY;
    servidor_addr.sin_port = htons(puertoEscucha);

    if (bind(servidor_socket, (struct sockaddr*)&servidor_addr, sizeof(servidor_addr)) < 0) {
        std::cerr << "Error al enlazar el socket al puerto.\n";
        close(servidor_socket);
        return;
    }

    if (listen(servidor_socket, 5) < 0) {
        std::cerr << "Error al poner en escucha el socket.\n";
        close(servidor_socket);
        return;
    }

    while (true) {
        sockaddr_in cliente_addr{};
        socklen_t cliente_len = sizeof(cliente_addr);
        int cliente_socket = accept(servidor_socket, (struct sockaddr*)&cliente_addr, &cliente_len);
        if (cliente_socket < 0) {
            std::cerr << "Error al aceptar la conexión.\n";
            continue;
        }

        char buffer[1024];
        ssize_t bytes_recibidos;
        while ((bytes_recibidos = recv(cliente_socket, buffer, sizeof(buffer) - 1, 0)) > 0) {
            buffer[bytes_recibidos] = '\0';
            emit mensajeRecibido(QString::fromUtf8(buffer));
        }

        close(cliente_socket);
    }

    close(servidor_socket);
}
