#include "studentinfosearch.h"

studentInfoSearch::studentInfoSearch(QWidget *parent) :
    QWidget(parent)
{
    msg = MessageBox::getInstance();
    client = TcpClient::getSocket();
    // 学生信息显示窗口初始化
    studentInfoDia = new QDialog(this);
    getName = new QLabel(studentInfoDia);
    getName->move(20,20);
    getId = new QLabel(studentInfoDia);
    getId->move(20,70);
    getBirthday = new QLabel(studentInfoDia);
    getBirthday->move(20,120);
    getBirthplace = new QLabel(studentInfoDia);
    getBirthplace->move(20,170);
    studentInfoDia->setFixedSize(300,220);


    // 查询窗口初始化
    this->setWindowTitle("学生档案查询");
    this->setFixedSize(300,80);
    studentId = new QLineEdit(this);
    id = new QLabel("请输入查询学号：",this);
    id->move(20,24);
    studentId->move(120,20);
    QPushButton *searchBtn = new QPushButton("查询",this);
    QPushButton *cancelBtn = new QPushButton("取消",this);
    searchBtn->move(20,50);
    cancelBtn->move(200,50);
    connect(searchBtn, &QPushButton::clicked, this, &studentInfoSearch::dosearch);
    connect(cancelBtn, &QPushButton::clicked, this, &studentInfoSearch::close);
}


void studentInfoSearch::dosearch()
{
    QNetworkReply* reply = client->send_msg_post(TcpClient::SEARCH, studentId->text());
    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray data = reply->readAll();
        QString dataStr = QString::fromUtf8(data); // QByteArray 转 QString
//        qDebug() << dataStr;
        if(dataStr[0] == '0')
        {
            msg->getMsgBox()->critical(this,"警告","查询失败！");
        }
        else if(dataStr[0] == '1')
        {

            QStringList strList = dataStr.split("&"); // 字符串分割
            getName->setText("姓名：" + strList[1]);
            getId->setText("学号：" + strList[2]);
            getBirthday->setText("出生日期：" + strList[3]);
            getBirthplace->setText("出生地：" + strList[4]);
            studentInfoDia->show(); // 打开信息窗口
        }
        else
        {
            msg->getMsgBox()->information(this,"提示","查无此人，请确认学号正确！");
        }

    });
    //在使用QNetworkReply之后需要释放内存
    connect(reply, &QNetworkReply::finished, reply, &QObject::deleteLater);
}


studentInfoSearch* studentInfoSearch::getInstance()
{
    static studentInfoSearch sis;
    return &sis;
}

studentInfoSearch::~studentInfoSearch()
{
    qDebug() << "~studentInfoSearch";
}
