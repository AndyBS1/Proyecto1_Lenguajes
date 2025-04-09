#ifndef HILORECEPTOR_H
#define HILORECEPTOR_H

#include <QThread>
#include <QString>

class HiloReceptor : public QThread {
    Q_OBJECT

public:
    HiloReceptor(int puerto, QObject *parent = nullptr);
    void run() override;

signals:
    void mensajeRecibido(QString mensaje);

private:
    int puertoEscucha;
};

#endif
