#include "mysocket.h"

MySocket::MySocket()
{
    m_name="",
    tcp=new QTcpSocket;
    isConnected=true;

}
