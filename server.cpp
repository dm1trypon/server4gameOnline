#include "server.h"

#include <QObject>
#include <QDataStream>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

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
    clientList << pClientSocket;
    qDebug() << "Client" << pClientSocket << "has been connected!";
    connect(pClientSocket, SIGNAL(disconnected()), this, SLOT(slotDisconnected()));
    connect(pClientSocket, SIGNAL(readyRead()), this, SLOT(slotReadClient()));
}

void Server::slotDisconnected()
{
    QTcpSocket* pClientSocket = static_cast<QTcpSocket*>(sender());
    removeClientData(pClientSocket);
    qDebug() << "Client" << pClientSocket << "has been disconnected!";
    pClientSocket->deleteLater();
}

void Server::removeClientData(QTcpSocket* pClientSocket)
{
    int idx = clientList.indexOf(pClientSocket);
    if (idx != -1)
    {
        clientList.removeAt(idx);
    }
    arrClients = _docClients.array();
    int i = 0;
    foreach (const QJsonValue & value, arrClients)
    {
        QJsonObject _objArr = value.toObject();
        if (onRemoveClient(_objArr, idx))
        {
            qDebug() << "Element of Array for delete found:" << _objArr;
            arrClients.removeAt(idx);
        }
        i++;
    }
    QJsonDocument docClients(arrClients);
    _docClients = docClients;
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
    clientsListToJson(pClientSocket, data);
    m_nNextBlockSize = 0;
    foreach(QTcpSocket* client, clientList)
    {
        qDebug() << client << data;
        sendToClient(client, data);
    }
}

void Server::clientsListToJson(QTcpSocket* pClientSocket, QString data)
{
    QJsonDocument docData = QJsonDocument::fromJson(data.toUtf8());
    QJsonObject objData = docData.object();
    if (onTypeData(objData))
    {
        int idx = clientList.indexOf(pClientSocket);
        QJsonObject objClients;
        objClients.insert(NICKNAME, objData.value(NICKNAME));
        objClients.insert(SOCKETIDX, idx);
        objClients.insert(POSX, objData.value(POSX));
        objClients.insert(POSY, objData.value(POSY));
        arrClients.push_back(objClients);
        QJsonDocument docClients(arrClients);
        _docClients = docClients;
        qDebug() << "JSON CLIENTS:" <<_docClients;
    }
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

bool Server::onRemoveClient(QJsonObject _objArr, int idx)
{
    return _objArr.value(SOCKETIDX) == idx;
}

bool Server::onTypeData(QJsonObject objData)
{
    return objData.value(TYPE) == CONNECTION;
}


