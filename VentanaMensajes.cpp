#include "VentanaMensajes.h"
#include "enviarmensaje.h"
#include <QHBoxLayout>

InterfazMensajes::InterfazMensajes(QString origen, QString destino, QWidget *parent) 
    : QWidget(parent), usuarioOrigen(origen), usuarioDestino(destino) {
    setWindowTitle("Mensajes");
    resize(400, 600);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Parte de arriba
    usuarioLabel = new QLabel("Chat: " + destino);
    usuarioLabel->setAlignment(Qt::AlignCenter);
    usuarioLabel->setStyleSheet("font-size: 18px; font-weight: bold; padding: 10px;");

    // Mensajes
    contenedorMensajes = new QWidget();
    layoutMensajes = new QVBoxLayout(contenedorMensajes);
    layoutMensajes->setAlignment(Qt::AlignTop);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(contenedorMensajes);

    // Entrada de mensajes
    mensajeInput = new QTextEdit();
    mensajeInput->setFixedHeight(50);

    QPushButton *botonEnviar = new QPushButton("Enviar");
    connect(botonEnviar, &QPushButton::clicked, this, &InterfazMensajes::enviarMensaje);

    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->addWidget(mensajeInput);
    inputLayout->addWidget(botonEnviar);

    mainLayout->addWidget(usuarioLabel);
    mainLayout->addWidget(scrollArea);
    mainLayout->addLayout(inputLayout);
}

void InterfazMensajes::enviarMensaje() {
    QString mensaje = mensajeInput->toPlainText();
    if (!mensaje.isEmpty()) {
        agregarMensaje(mensaje, true);
        mensajeInput->clear();

        enviar_mensaje(usuarioOrigen.toStdString(), usuarioDestino.toStdString(), mensaje.toStdString());
    }
}

void InterfazMensajes::agregarMensaje(QString mensaje, bool enviado) {
    QLabel *mensajeLabel = new QLabel(mensaje);
    mensajeLabel->setWordWrap(true);
    mensajeLabel->setMaximumWidth(300);

    if (enviado) {
        mensajeLabel->setStyleSheet("background-color: green; color: white; padding: 5px; border-radius: 5px;");
    } else {
        mensajeLabel->setStyleSheet("background-color: darkgray; color: white; padding: 5px; border-radius: 5px;");
    }

    layoutMensajes->addWidget(mensajeLabel);
}