#ifndef SNTPSERVER_H
#define SNTPSERVER_H

#include <QObject>
#include <QUdpSocket>
#include <QDateTime>
#include <QDebug>
#include <QMetaType>
// Q_DECLARE_METATYPE(CZigMessageInfo)


class SNTPServer : public QObject
{
    Q_OBJECT
public:
    explicit SNTPServer(QObject *parent = nullptr);
private slots:
    void processRequest();
    void onConnected();
    void onStateChanged(QAbstractSocket::SocketState newstate);
signals:
private:
    QUdpSocket *udpSocket;

};

#endif // SNTPSERVER_H
