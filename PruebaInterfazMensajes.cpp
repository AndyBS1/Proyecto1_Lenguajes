#include "VentanaMensajes.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);  // Crea la aplicación Qt

    // Aquí se muestra la ventana de la interfaz gráfica con un nombre de usuario de ejemplo
    InterfazMensajes ventana("UsuarioDestino");  // 'UsuarioDestino' es solo un ejemplo
    ventana.show();  // Muestra la ventana en pantalla

    return app.exec();  // Ejecuta el bucle de eventos de Qt para mantener la ventana abierta
}
