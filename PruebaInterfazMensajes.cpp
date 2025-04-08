#include "VentanaMensajes.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    InterfazMensajes ventana("UsuarioDestino");
    ventana.show();
    return app.exec();
}