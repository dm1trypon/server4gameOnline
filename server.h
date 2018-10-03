#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QNetworkSession>
#include <QFile>

class Server: public QObject
{
    Q_OBJECT
public:
    Server(int nPort);
public slots:
    virtual void slotNewConnection();
    void slotReadClient();
private:
    QList<QTcpSocket*> clientList;
    QTcpServer* m_ptcpServer;
    quint16 m_nNextBlockSize;
    void sendToClient(QTcpSocket* pSocket, const QString& data);
    void errorMessage();
};

#endif // SERVER_H
