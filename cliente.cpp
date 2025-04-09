#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h>
#include "enviarmensaje.h"
#include "recibirmensaje.h"
#include <QApplication>
#include "VentanaMensajes.h"

bool autenticar_usuario(const std::string& usuario, const std::string& contrasena, int &puerto) {
    std::ifstream archivo("usuarios.txt");
    if (!archivo.is_open()) {
        std::cerr << "No se pudo abrir usuarios.txt\n";
        return false;
    }

    std::string linea;
    while (std::getline(archivo, linea)) {
        std::istringstream iss(linea);
        std::string usuario_archivo, ip, pass;
        int puerto_archivo;
        if (iss >> usuario_archivo >> ip >> puerto_archivo >> pass) {
            if (usuario_archivo == usuario && pass == contrasena) {
                puerto = puerto_archivo;  // Asignar el puerto encontrado
                return true;
            }
        }
    }

    return false;
}

int main(int argc, char *argv[]) {
    std::string usuario_origen;
    std::string contrasena;

    std::cout << "Usuario: ";
    std::cin >> usuario_origen;
    std::cout << "Contraseña: ";
    std::cin >> contrasena;
    std::cin.ignore();

    int puerto_asignado = 0;
    if (!autenticar_usuario(usuario_origen, contrasena, puerto_asignado)) {
        std::cerr << "Credenciales incorrectas.\n";
        return 1;
    }

    std::cout << "Puerto asignado: " << puerto_asignado << "\n";  // Mostrar el puerto asignado

    std::string usuario_destino;
    std::cout << "Para: ";
    std::cin >> usuario_destino;
    std::cin.ignore(); // Se limpia el buffer

    // Configuración de la conexión al servidor (Siempre al puerto 8080)
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in servidor_address;
    servidor_address.sin_family = AF_INET;
    servidor_address.sin_port = htons(8080); // Siempre al puerto 8080 del servidor
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
        //recibir_mensaje(puerto_asignado);  // Escuchar en el puerto asignado (por ejemplo, 5000)
    } else {  // Proceso padre: enviar mensaje
        QApplication app(argc, argv);

        // Pasar origen y destino a la interfaz
        InterfazMensajes ventana(
            QString::fromStdString(usuario_origen),
            QString::fromStdString(usuario_destino),
            puerto_asignado
        );
        ventana.show();

        return app.exec(); 
    }

}