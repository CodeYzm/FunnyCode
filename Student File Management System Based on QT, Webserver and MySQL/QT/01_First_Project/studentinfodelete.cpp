#include "studentinfodelete.h"

StudentInfoDelete::StudentInfoDelete(QWidget *parent) :
    QWidget(parent)
{
    msg = MessageBox::getInstance();
    client = TcpClient::getSocket();
    // 查询删除界面
    this->setWindowTitle("学生档案搜索");
    this->setFixedSize(300,80);
    studentId = new QLineEdit(this);
    id = new QLabel("请输入学号：",this);
    id->move(20,24);
    studentId->move(120,20);
    QPushButton *deleteBtn = new QPushButton("删除",this);
    QPushButton *cancelBtn = new QPushButton("取消",this);
    deleteBtn->move(20,50);
    cancelBtn->move(200,50);
    connect(deleteBtn, &QPushButton::clicked, this, &StudentInfoDelete::dodelete);
    connect(cancelBtn, &QPushButton::clicked, this, &StudentInfoDelete::close);
}

StudentInfoDelete* StudentInfoDelete::getInstance()
{
    static StudentInfoDelete sid;
    return &sid;
}

void StudentInfoDelete::dodelete()
{
    QNetworkReply* reply = client->send_msg_post(TcpClient::DELETE, studentId->text());
    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray data = reply->readAll();
        QString dataStr = QString::fromUtf8(data); // QByteArray 转 QString
        qDebug() << dataStr;
        if(dataStr[0] == '0')
        {
            msg->getMsgBox()->critical(this,"警告","删除失败！");
        }
        else if(dataStr[0] == '1')
        {
            msg->getMsgBox()->information(this,"提示","已成功删除！");
        }
        else
        {
            msg->getMsgBox()->information(this,"提示","查无此人，请确认学号正确！");
        }

    });
    //在使用QNetworkReply之后需要释放内存
    connect(reply, &QNetworkReply::finished, reply, &QObject::deleteLater);
}


StudentInfoDelete::~StudentInfoDelete()
{
    qDebug() << "~StudentInfoDelete";
}
