#ifndef VENTANAMENSAJES_H
#define VENTANAMENSAJES_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QThread>

class HiloReceptor;

class InterfazMensajes : public QWidget {
    Q_OBJECT

public:
    InterfazMensajes(QString usuarioOrigen, QString usuarioDestino, int puertoEscucha, QWidget *parent = nullptr);
    ~InterfazMensajes();

private slots:
    void enviarMensaje();
    void mostrarMensaje(QString mensaje);

private:
    QLabel *usuarioLabel;
    QTextEdit *mensajeInput;
    QVBoxLayout *layoutMensajes;
    QWidget *contenedorMensajes;
    QString usuarioOrigen;
    QString usuarioDestino;
    QScrollArea *scrollArea;
    HiloReceptor *hiloReceptor;

    void agregarMensaje(QString mensaje, bool enviado);
    QString extraerContenidoMensaje(const QString& mensajeCompleto);
    
};

#endif
