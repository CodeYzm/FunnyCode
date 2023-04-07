#include "tcpclient.h"

TcpClient::TcpClient(QObject *parent) :
    QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    ip = "192.168.59.128";
    port = 9006;
}

TcpClient* TcpClient::getSocket()
{
    static TcpClient clientTcp;
    return &clientTcp;
}

QNetworkReply* TcpClient::send_msg_post(HANDLE handle,QString msg)
{
    // 设置请求内容
    QByteArray data;
    data.append(msg);
    QString m_url = "http://" + ip + ":" + QString::number(port)+ "/" +QString::number(handle);
    QUrl url(m_url);
    qDebug() << m_url;
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    // 发送 POST 请求
    QNetworkReply *reply = manager->post(request, data);
    return reply;
}

TcpClient::~TcpClient()
{
    qDebug() << "~TcpClient";
}
