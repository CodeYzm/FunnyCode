#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include<QTcpSocket>
#include<QtNetwork>

// 通信类，用于向服务器发送HTTP请求
class TcpClient : public QObject
{
    Q_OBJECT
public:
    enum HANDLE // 使用枚举整形常量作为与服务器沟通的句柄，分别对应7种操作
    {
        REGISTER = 0, // 注册
        LOGIN, // 登录
        CREATE, // 新建档案
        SEARCH, // 搜索档案
        MODIFY, // 修改档案
        DELETE, // 删除档案
        CLEAR // 清空档案
    };
    QNetworkReply* send_msg_post(HANDLE handle, QString msg); // 给服务端发送消息（全部用POST请求）
    static TcpClient* getSocket();

private:
    explicit TcpClient(QObject *parent = 0);
    ~TcpClient();
    // 记录服务器IP地址和端口号
    QString ip;
    int port;
    QNetworkAccessManager* manager;
signals:

public slots:

};

#endif // TCPCLIENT_H
