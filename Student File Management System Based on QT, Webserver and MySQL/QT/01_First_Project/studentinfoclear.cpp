#include "studentinfoclear.h"

StudentInfoClear::StudentInfoClear(QWidget *parent) :
    QWidget(parent)
{
    msg = MessageBox::getInstance();
    client = TcpClient::getSocket();
}

StudentInfoClear* StudentInfoClear::getInstance()
{
    static StudentInfoClear sic;
    return &sic;
}

void StudentInfoClear::doclear()
{
    // 清空数据前警告操作者
    int ret = msg->getMsgBox()->critical(this,"警告","请确认是否清空学生档案数据，一经清空无法恢复！！！",QMessageBox::Yes|QMessageBox::Cancel);
    if(ret == QMessageBox::Yes) // 操作者确定清空，则执行操作，向服务器发起清空数据表的请求
    {
        QNetworkReply* reply = client->send_msg_post(TcpClient::CLEAR,"");
        connect(reply, &QNetworkReply::finished, this, [=]() {
            QByteArray data = reply->readAll();
            QString dataStr = QString::fromUtf8(data); // QByteArray 转 QString
            qDebug() << dataStr;
            if(dataStr[0] == '0')
            {
                msg->getMsgBox()->critical(this,"警告","数据清空失败！");
            }
            else if(dataStr[0] == '1')
            {
                msg->getMsgBox()->information(this,"提示","已成功清空学生档案！");
            }
            else
            {
                msg->getMsgBox()->information(this,"提示","数据库中不存在学生档案！");
            }

        });
        //在使用QNetworkReply之后需要释放内存
        connect(reply, &QNetworkReply::finished, reply, &QObject::deleteLater);
    }
    else
    {
        this->close();
    }
}

StudentInfoClear::~StudentInfoClear()
{
    qDebug() << "~StudentInfoClear";
}
