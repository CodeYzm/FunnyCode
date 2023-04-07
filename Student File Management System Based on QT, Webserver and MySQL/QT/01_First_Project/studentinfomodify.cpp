#include "studentinfomodify.h"
const QString SPROVINCE = "省";
const QString SCITY = "市";
const QString SCOUNTY1 = "县";
const QString SCOUNTY2 = "区";
const QString SYEAR = "年";
const QString SMONTH = "月";
const QString SDAY = "日";
// 初始化正则匹配
const QString birthday_pattern = "^[0-9]+年[0-9]+月[0-9]+日$"; // 数字年数字月数字日
const QRegularExpression birthday_regExp(birthday_pattern);
const QString birthplace_pattern = "^[\u4e00-\u9fa5]+省[\u4e00-\u9fa5]+市[\u4e00-\u9fa5]+[县区]$"; // 中文省中文市中文区
const QRegularExpression birthplace_regExp(birthplace_pattern);

StudentInfoModify::StudentInfoModify(QWidget *parent) :
    QWidget(parent)
{
    msg = MessageBox::getInstance();
    // 学生信息显示窗口初始化
    studentInfoDia = new QDialog(this);
    oldMsg = new QLabel("原始信息",studentInfoDia);
    oldMsg->move(20,20);
    getName = new QLabel(studentInfoDia);
    getName->move(20,50);
    getId = new QLabel(studentInfoDia);
    getId->move(20,100);
    getBirthday = new QLabel(studentInfoDia);
    getBirthday->move(20,150);
    getBirthplace = new QLabel(studentInfoDia);
    getBirthplace->move(20,200);
    QPushButton* modifyBtn = new QPushButton("提交", studentInfoDia);
    modifyBtn->move(50,250);
    QPushButton* cancelModifyBtn = new QPushButton("取消", studentInfoDia);
    cancelModifyBtn->move(330,250);
    connect(modifyBtn, &QPushButton::clicked, this, &StudentInfoModify::domodify);
    connect(cancelModifyBtn, &QPushButton::clicked, studentInfoDia, &QDialog::close);

    // 新信息
    newMsg = new QLabel("新信息",studentInfoDia);
    newMsg->move(300,20);
    newName = new QLineEdit(studentInfoDia);
    newName->move(300,50);
    newId = new QLineEdit(studentInfoDia);
    newId->setText("学号不可修改！！！");
    newId->setReadOnly(true); //学号栏不可修改
    newId->move(300,100);
    newBirthday = new QLineEdit(studentInfoDia);
    newBirthday->move(300,150);
    newBirthPlace = new QLineEdit(studentInfoDia);
    newBirthPlace->move(300,200);
    studentInfoDia->setFixedSize(450,280);


    // 查询界面
    client = TcpClient::getSocket();
    this->setWindowTitle("学生档案搜索");
    this->setFixedSize(300,80);
    studentId = new QLineEdit(this);
    id = new QLabel("请输入学号：",this);
    id->move(20,24);
    studentId->move(120,20);
    QPushButton *searchBtn = new QPushButton("查询",this);
    QPushButton *cancelBtn = new QPushButton("取消",this);
    searchBtn->move(20,50);
    cancelBtn->move(200,50);
    connect(searchBtn, &QPushButton::clicked, this, &StudentInfoModify::dosearch);
    connect(cancelBtn, &QPushButton::clicked, this, &StudentInfoModify::close);
}

StudentInfoModify* StudentInfoModify::getInstance()
{
    static StudentInfoModify sim;
    return &sim;
}

// 搜索并展示学生档案信息
void StudentInfoModify::dosearch()
{
    QNetworkReply* reply = client->send_msg_post(TcpClient::SEARCH, studentId->text());
    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray data = reply->readAll();
        QString dataStr = QString::fromUtf8(data); // QByteArray 转 QString
        qDebug() << dataStr;
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

// 转成utf8再进行正则匹配检查输入格式
bool StudentInfoModify::birthday_check(QString birthday)
{
    return birthday_regExp.match(QString::fromUtf8(birthday.toUtf8())).hasMatch();
}
bool StudentInfoModify::birthplace_check(QString birthplace)
{
    return birthplace_regExp.match(QString::fromUtf8(birthplace.toUtf8())).hasMatch();
}

void StudentInfoModify::domodify()
{
    QString dayMsg = newBirthday->text();
    QString placeMsg = newBirthPlace->text();
    if(birthday_check(dayMsg) && birthplace_check(placeMsg)) // 如果输入格式正确，则向服务器发起修改请求
    {
        QRegularExpression re("\\d+"); // 匹配数字字符的正则表达式
        QRegularExpressionMatchIterator it = re.globalMatch(getId->text()); // 全局匹配
        QRegularExpressionMatch match = it.next();
        QString numStr = match.captured(0); // 获取匹配到的数字字符串

        QString modifyMsg = newName->text()+"&"+numStr+"&"+dayMsg+"&"+placeMsg;
        qDebug() << modifyMsg;
        QNetworkReply* reply = client->send_msg_post(TcpClient::MODIFY, modifyMsg);
        connect(reply, &QNetworkReply::finished, this, [=]() {
            QByteArray data = reply->readAll();
            QString dataStr = QString::fromUtf8(data); // QByteArray 转 QString
            qDebug() << dataStr;
            if(dataStr[0] == '0')
            {
                msg->getMsgBox()->critical(this,"警告","修改失败！");
                studentInfoDia->close();
            }
            else if(dataStr[0] == '1')
            {

                msg->getMsgBox()->information(this,"提示","修改成功！");
                studentInfoDia->close();
            }
            else
            {
                msg->getMsgBox()->information(this,"提示","查无此人，请确认学号正确！");
            }

        });
        //在使用QNetworkReply之后需要释放内存
        connect(reply, &QNetworkReply::finished, reply, &QObject::deleteLater);
    }
    else // 如果输入格式不对，则提示检查输入格式
    {
        msg->getMsgBox()->critical(this,"警告","请检查输入格式！");
    }
}

StudentInfoModify::~StudentInfoModify()
{
    qDebug() << "~StudentInfoModify";
}

