#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QNetworkSession>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>

class Server: public QObject
{
    Q_OBJECT
public:
    Server(int nPort);
public slots:
    virtual void slotNewConnection();
    void slotReadClient();
private slots:
    void slotDisconnected();
private:
    const QString CLIENTS_LIST = "clientsList";
    const QString POSY = "posY";
    const QString POSX = "posX";
    const QString MOVE = "move";
    const QString NICKNAME = "nickName";
    const QString TYPE = "type";
    const QString CONNECTION = "connection";
    const QString SOCKETIDX = "socketIdx";
    QJsonDocument _docClients;
    QJsonArray arrClients;
    QList<QTcpSocket*> clientList;
    QTcpServer* m_ptcpServer;
    quint16 m_nNextBlockSize;
    void sendToClient(QTcpSocket* pSocket, const QString& data);
    void errorMessage();
    QString clientsListToJson(QTcpSocket *pClientSocket, QString data);
    void removeClientData(QTcpSocket *pClientSocket);
    bool onRemoveClient(QJsonObject _objArr, int idx);
    bool onTypeData(QJsonObject objData);
    void senderOut(QString data);
};

#endif // SERVER_H
