#ifndef MYSOCKET_H
#define MYSOCKET_H
#include <QString>
#include <QTcpSocket>

class MySocket
{
public:
    MySocket();
    QString m_name;
    QTcpSocket *tcp;
    bool isConnected;
};


#endif // MYSOCKET_H
