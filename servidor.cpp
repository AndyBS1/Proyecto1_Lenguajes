#include <iostream>
#include <map>
#include <fstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <signal.h>
#include <set>

struct Usuario {
    std::string ip; // Direccion ip
    int puerto;  // Puerto del usuario
    std::string contrasena; 
};

std::map<std::string, Usuario> usuarios;  // Mapa para almacenar a los usuarios
const std::string ARCHIVO_USUARIOS = "usuarios.txt";  // Archivo donde se almacenan los usuarios

void cargar_usuarios() { // Funcion para mostrar la lista de los usuarios que se han registrado en el servidor
    std::ifstream archivo(ARCHIVO_USUARIOS);
    if (!archivo.is_open()) return; // Si no logra abrir el archivo se sale de la funcion

    std::string usuario, ip, contrasena;
    int puerto;
    while (archivo >> usuario >> ip >> puerto >> contrasena) {
        usuarios[usuario] = {ip, puerto, contrasena};
    }
    archivo.close();
}

void guardar_usuarios() {  // Funcion para guardar los usuarios en el archivo txt
    std::ofstream archivo(ARCHIVO_USUARIOS);
    for (const auto& [usuario, datos] : usuarios) {
        archivo << usuario << " " << datos.ip << " " << datos.puerto << " " << datos.contrasena << "\n";
    }
    archivo.close();
}

void manejar_cliente(int cliente_sock) { // Manejar las solicitudes de los clientes
    cargar_usuarios();
    char buffer[1024] = {0};  
    ssize_t bytes_leidos = read(cliente_sock, buffer, sizeof(buffer)-1);
    if (bytes_leidos <= 0) {  // Si no lee nada o la conexion con el server esta cerrada, cierra el socket
        std::cerr << "Error leyendo datos o conexión cerrada\n";
        close(cliente_sock);
        return;
    }
    buffer[bytes_leidos] = '\0';

    std::string datos(buffer);  
    std::cout << "Mensaje recibido: " << datos << std::endl; // Para verificar que las cosas lleguen al server
    // Para listar los usuarios que hay registrados
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

    // Extraemos los campos del mensaje
    size_t pos1 = datos.find("|");
    size_t pos2 = datos.find("|", pos1+1);
    size_t pos3 = datos.find("|", pos2+1);

    if (pos1 == std::string::npos || pos2 == std::string::npos || pos3 == std::string::npos){
        std::cerr << "Error: Formato de mensaje inválido\n";
        close(cliente_sock);
        return;
    }    

    std::string comando = datos.substr(0, pos1);
    std::string usuario = datos.substr(pos1+1, pos2-pos1-1);
    std::string ip = datos.substr(pos2+1, pos3-pos2-1);
    std::string contrasena = datos.substr(pos3+1);

    // Asignación de puerto
    int puerto_asignado = 5000; // Puerto inicial
    std::set<int> puertos_ocupados;

    for (const auto& [nombre, datos] : usuarios) {
        puertos_ocupados.insert(datos.puerto);
    }

    while (puertos_ocupados.count(puerto_asignado)) {
        puerto_asignado++;
    }

    // Para el registro
    if (comando == "REGISTRO") {
        if (usuarios.find(usuario) != usuarios.end()) {
            std::cerr << "Intento de registro duplicado: " << usuario << "\n";
            send(cliente_sock, "ERROR|Usuario ya existe", 23, 0);
            close(cliente_sock);
        } else {
            usuarios[usuario] = {ip, puerto_asignado, contrasena};  
            std::cout << "Usuario registrado: " << usuario << " IP: " << ip << " Puerto: " << puerto_asignado << "\n";
            
            std::string respuesta = "OK|Registro exitoso|PUERTO|" + std::to_string(puerto_asignado);
            send(cliente_sock, respuesta.c_str(), respuesta.size(), 0);
            guardar_usuarios();
            close(cliente_sock);
        }
    }
    // Para el login
    else if (comando == "LOGIN") {
        cargar_usuarios();
        std::cout << "Intentando login: " << usuario << " con contraseña: " << contrasena << "\n";
    
        auto it = usuarios.find(usuario);
        if (it != usuarios.end()) {
            std::cout << "Usuario encontrado. Contraseña esperada: " << it->second.contrasena << "\n";
    
            if (it->second.contrasena == contrasena) {
                std::cout << "Inicio de sesión exitoso para usuario: " << usuario << "\n";
                send(cliente_sock, "OK|Inicio de sesión exitoso", 28, 0);
            } else {
                std::cerr << "Contraseña incorrecta\n";
                send(cliente_sock, "ERROR|Credenciales inválidas", 29, 0);
            }
        } else {
            std::cerr << "Usuario no encontrado\n";
            send(cliente_sock, "ERROR|Credenciales inválidas", 29, 0);
        }
    }
    
    else if (comando == "MENSAJE") {
        // Extraemos el nombre del usuario origen
        std::string usuario_origen = usuario;  // Usuario origen (el que envía el mensaje)
    
        // Extraemos el nombre del usuario destino correctamente
        std::string usuario_destino = datos.substr(pos2 + 1, pos3 - pos2 - 1);  // Extraemos el usuario destino
        std::cout << "Usuario destino: " << usuario_destino << std::endl; // Depuración: Verificar el usuario destino
    
        // El mensaje es todo lo que está después del último "|"
        std::string mensaje = datos.substr(pos3 + 1);
        std::cout << "Mensaje: " << mensaje << std::endl; // Depuración: Verificar el mensaje
    
        // Buscar el usuario destino en el mapa
        auto it = usuarios.find(usuario_destino);
        if (it != usuarios.end()) {
            // Si el usuario destino existe, obtener la IP y el puerto asignado
            std::string ip_destino = it->second.ip;
            int puerto_destino = it->second.puerto;
    
            std::cout << "Conectando al destinatario: " << usuario_destino << " IP: " << ip_destino << " Puerto: " << puerto_destino << std::endl;
    
            // Crear un socket para conectarse al puerto del usuario destino
            int sock_destino = socket(AF_INET, SOCK_STREAM, 0);
            if (sock_destino < 0) {
                std::cerr << "Error creando el socket para el destino\n";
                return;
            }
    
            sockaddr_in direccion_destino;
            direccion_destino.sin_family = AF_INET;
            direccion_destino.sin_port = htons(puerto_destino);
            inet_pton(AF_INET, ip_destino.c_str(), &direccion_destino.sin_addr);
    
            // Intentar conectarse al puerto del usuario destino
            if (connect(sock_destino, (struct sockaddr *)&direccion_destino, sizeof(direccion_destino)) < 0) {
                std::cerr << "Error al conectar con el usuario destino en puerto " << puerto_destino << std::endl;
                close(sock_destino);
                send(cliente_sock, "ERROR|No se pudo conectar al usuario destino", 42, 0);
                return;
            }
    
            // Construir el mensaje final y enviarlo
            std::string mensaje_final = "MENSAJE|De: " + usuario_origen + "|A: " + usuario_destino + "|Mensaje: " + mensaje;
            std::cout << "Mensaje final a enviar: " << mensaje_final << std::endl; // Depuración: Verificar el mensaje final
            send(sock_destino, mensaje_final.c_str(), mensaje_final.size(), 0);
    
            std::cout << "Mensaje enviado de: " << usuario_origen << " a: " << usuario_destino << " -> " << mensaje << "\n";
    
            // Cerrar el socket después de enviar el mensaje
            close(sock_destino); 
        } else {
            // Si no se encuentra el usuario destino
            send(cliente_sock, "ERROR|Usuario destino no encontrado", 34, 0);
        }
    }

    close(cliente_sock);
}

int main() {
    cargar_usuarios(); // Carga los usuarios del archivo
    int server_fd = socket(AF_INET, SOCK_STREAM, 0); // Crea el socket para el server
    if (server_fd < 0) { // Validación por si hay un error
        std::cerr << "Error creando el socket: " << strerror(errno) << "\n";
        exit(EXIT_FAILURE);
    }

    sockaddr_in address; // Estructura para la dirección del server
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080); // Puerto definido

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 5);
    signal(SIGCHLD, SIG_IGN);

    std::cout << "Servidor iniciado en puerto 8080\n"; // Mensaje para verificar que se conectó al server

    while(true) {
        int cliente_sock = accept(server_fd, nullptr, nullptr);
        if (cliente_sock >= 0) {
            if (fork() == 0) { // Se crea un proceso hijo para atender al cliente
                manejar_cliente(cliente_sock); // Maneja las solicitudes del usuario
                exit(0);
            }
        }
    }

    close(server_fd);
    return 0;
}
