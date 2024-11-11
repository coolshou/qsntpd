#include "sntpserver.h"
#include <QNetworkDatagram>
#include <QByteArray>
#include <QCoreApplication>



SNTPServer::SNTPServer(QObject *parent)
    : QObject{parent}
{
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
    udpSocket = new QUdpSocket(this);
    // if (!udpSocket->bind(123, QUdpSocket::ShareAddress|| QUdpSocket::ReuseAddressHint)) {
    if (!udpSocket->bind(123, QUdpSocket::ShareAddress)) {
        qDebug() << "Failed to bind to SNTP port 123.";
        return;
    }
    connect(udpSocket, &QUdpSocket::readyRead, this, &SNTPServer::processRequest);
    connect(udpSocket, &QUdpSocket::stateChanged, this, &SNTPServer::onStateChanged);
    // qDebug() << "SNTP server started, listening on port 123...";
}

void SNTPServer::processRequest()
{
    while (udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSocket->receiveDatagram();
        QByteArray request = datagram.data();

        // Prepare SNTP response
        QByteArray response(48, 0);
        response[0] = 0x1C;  // LI, Version, Mode fields: No warning, version 3, server mode
        // Set LI, Version, and Mode (LI=0 (no warning), Version=3, Mode=4 (server))
        response[0] = 0x1C;
        // Set Stratum to 1 (primary server)
        response[1] = 1;
        // Set Poll Interval (log2 of max interval, typically between 4 and 10)
        response[2] = 6;
        // Precision (set to a typical precision for SNTP, -20)
        response[3] = -20;

        // Current time in seconds since 1900 (epoch time + offset to 1900)
        quint32 currentTime = QDateTime::currentDateTimeUtc().toSecsSinceEpoch() + 2208988800U;
        QByteArray secondsArray(4, 0);
        for (int i = 3; i >= 0; --i) {
            secondsArray[i] = (currentTime >> (8 * (3 - i))) & 0xFF;
        }
        // Set Reference Timestamp (set to current time)
        response.replace(16, 4, secondsArray);
        // Set Receive Timestamp (time of receiving the client's request)
        response.replace(32, 4, secondsArray);
        // Transmit Timestamp (same as current time)
        response.replace(40, 4, secondsArray);
        // Set Originate Timestamp (copied from the client's request at bytes 40–43)
        if (request.size() >= 48) {  // Ensure the request is of expected SNTP packet size
            QByteArray originateTimestamp = request.mid(40, 4);
            response.replace(24, 4, originateTimestamp);
        }

        // Send response back to client
        udpSocket->writeDatagram(response, datagram.senderAddress(), datagram.senderPort());
        qDebug() << "Responded to SNTP request from" << datagram.senderAddress().toString();
    }
}

void SNTPServer::onConnected()
{

}

void SNTPServer::onStateChanged(QAbstractSocket::SocketState newstate)
{
    qDebug() << "onStateChanged:" << newstate;
    if (newstate==QAbstractSocket::BoundState){
        qDebug() << "SNTP server started, listening on port 123...";
    }
}
