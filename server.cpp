#include "server.h"

#include <QObject>
#include <QDataStream>
#include <QTcpSocket>

Server::Server(int nPort) : m_nNextBlockSize(0)
{
    m_ptcpServer = new QTcpServer(this);
    if (!m_ptcpServer->listen(QHostAddress::Any, static_cast<quint16>(nPort)))
    {
        errorMessage();
    }
    qDebug() << "Server has been started on" << QHostAddress::Any << ":" << nPort;
    connect(m_ptcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
}

void Server::slotNewConnection()
{
    QTcpSocket* pClientSocket = m_ptcpServer->nextPendingConnection();
    qDebug() << "Client" << pClientSocket << "has been connected!";
    connect(pClientSocket, SIGNAL(disconnected()), pClientSocket, SLOT(deleteLater()));
    connect(pClientSocket, SIGNAL(readyRead()), this, SLOT(slotReadClient()));
}

void Server::slotReadClient()
{
    QTcpSocket* pClientSocket = static_cast<QTcpSocket*>(sender());
    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_5_8);
    if (!m_nNextBlockSize)
    {
        in >> m_nNextBlockSize;
    }
    QString data;
    in >> data;
    qDebug() << data;
    m_nNextBlockSize = 0;
    sendToClient(pClientSocket, data);
}

void Server::sendToClient(QTcpSocket *pClientSocket, const QString &data)
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_8);
    out << quint16(0) << data;
    out.device()->seek(0);
    out << quint16(static_cast<quint16>(arrBlock.size()) - sizeof(quint16));
    pClientSocket->write(arrBlock);
}

void Server::errorMessage()
{
    qDebug() << "Server Error:" << "Unable to start the server:" << m_ptcpServer->errorString();
    m_ptcpServer->close();
    return;
}


