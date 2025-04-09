#ifndef VENTANAMENSAJES_H
#define VENTANAMENSAJES_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QScrollArea>

class InterfazMensajes : public QWidget {
    Q_OBJECT  

public:
explicit InterfazMensajes(QString usuarioOrigen, QString usuarioDestino, QWidget *parent = nullptr);

private slots:
    void enviarMensaje();

private:
    QLabel *usuarioLabel;
    QTextEdit *mensajeInput;
    QVBoxLayout *chatLayout;
    QWidget *contenedorMensajes;
    QVBoxLayout *layoutMensajes;
    QString usuarioOrigen;
    QString usuarioDestino;

    void agregarMensaje(QString mensaje, bool enviado);
};

#endif