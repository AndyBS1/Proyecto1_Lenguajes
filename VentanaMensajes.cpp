#include "VentanaMensajes.h"
#include "enviarmensaje.h"
#include "HiloReceptor.h"
#include <QHBoxLayout>

InterfazMensajes::InterfazMensajes(QString origen, QString destino, int puertoEscucha, QWidget *parent)
    : QWidget(parent), usuarioOrigen(origen), usuarioDestino(destino) {
    setWindowTitle("Mensajes");
    resize(400, 600);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    usuarioLabel = new QLabel("Chat: " + destino);
    usuarioLabel->setAlignment(Qt::AlignCenter);
    usuarioLabel->setStyleSheet("font-size: 18px; font-weight: bold; padding: 10px;");

    contenedorMensajes = new QWidget();
    layoutMensajes = new QVBoxLayout(contenedorMensajes);
    layoutMensajes->setAlignment(Qt::AlignTop);

    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(contenedorMensajes);

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

    // Inicializar el hilo receptor
    hiloReceptor = new HiloReceptor(puertoEscucha);
    connect(hiloReceptor, &HiloReceptor::mensajeRecibido, this, &InterfazMensajes::mostrarMensaje);
    hiloReceptor->start();
}

InterfazMensajes::~InterfazMensajes() {
    hiloReceptor->terminate();  // Finaliza el hilo al cerrar la ventana
    hiloReceptor->wait();
    delete hiloReceptor;
}

QString InterfazMensajes::extraerContenidoMensaje(const QString& mensajeCompleto) {
    QStringList partes = mensajeCompleto.split("Mensaje:");
    if (partes.size() == 2) {
        return partes[1].trimmed();  // Devuelve solo el mensaje, sin espacios
    }
    return mensajeCompleto; // Si no coincide el formato, se muestra todo
}

void InterfazMensajes::enviarMensaje() {
    QString mensaje = mensajeInput->toPlainText();
    if (!mensaje.isEmpty()) {
        agregarMensaje(mensaje, true);
        mensajeInput->clear();
        enviar_mensaje(usuarioOrigen.toStdString(), usuarioDestino.toStdString(), mensaje.toStdString());
    }
}

void InterfazMensajes::mostrarMensaje(QString mensaje) {
    agregarMensaje(mensaje, false);
}

void InterfazMensajes::agregarMensaje(QString mensaje, bool enviado) {
    // Solo mostrar el contenido después de "Mensaje:"
    QString contenido = extraerContenidoMensaje(mensaje);

    QLabel *mensajeLabel = new QLabel(contenido);
    mensajeLabel->setWordWrap(true);
    mensajeLabel->setMaximumWidth(300);

    if (enviado) {
        mensajeLabel->setStyleSheet("background-color: green; color: white; padding: 5px; border-radius: 5px;");
    } else {
        mensajeLabel->setStyleSheet("background-color: darkgray; color: white; padding: 5px; border-radius: 5px;");
    }

    QHBoxLayout *contenedorMensaje = new QHBoxLayout();
    if (enviado) {
        contenedorMensaje->addStretch();
        contenedorMensaje->addWidget(mensajeLabel);
    } else {
        contenedorMensaje->addWidget(mensajeLabel);
        contenedorMensaje->addStretch();
    }

    QWidget *wrapper = new QWidget();
    wrapper->setLayout(contenedorMensaje);
    layoutMensajes->addWidget(wrapper);
}
