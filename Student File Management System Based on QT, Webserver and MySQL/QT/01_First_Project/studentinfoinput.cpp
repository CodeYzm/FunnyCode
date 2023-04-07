#include "studentinfoinput.h"
#include<QDebug>
#include<QPushButton>

StudentInfoInput::StudentInfoInput(QWidget *parent) :
    QWidget(parent)
{
    msg = MessageBox::getInstance();
    client = TcpClient::getSocket();
    this->setWindowTitle("学生信息录入");
    // 完成录入界面的初始化
    std::pair<int,int> xy_pair = this->initLineEditPage();
    int inity = xy_pair.second, initx = xy_pair.first;

    // 录入界面的确认和取消按钮
    QPushButton* yesBtn = new QPushButton("确认",this);
    QPushButton* cancelBtn = new QPushButton("取消",this);
    int btny = inity + 40;
    yesBtn->move(20,btny);
    cancelBtn->move(initx-cancelBtn->width(),btny);
    this->setFixedSize(xy_pair.first,btny+cancelBtn->height());
    // 确认录入，则进行网络通信，向服务器发送信息，并由服务器更新到数据库
    connect(yesBtn, &QPushButton::clicked, this, &StudentInfoInput::tranInfo);
    // 取消录入，则关闭页面
    connect(cancelBtn, &QPushButton::clicked, this, &QWidget::close);

}

// @简介：信息录入界面设计
// @输入：无
// @输出：界面宽度，界面已经需求的高度
std::pair<int,int> StudentInfoInput::initLineEditPage()
{
    // 姓名：
    int lineEdit_bias_x = 75, lineEdit_bias_y = 25;
    QLabel* nameLabel = new QLabel(this);
    nameLabel->setText("姓名：");
    nameLabel->move(20,lineEdit_bias_y+4);
    getName = new QLineEdit(this);
    getName->move(lineEdit_bias_x,lineEdit_bias_y);

    // 学号：
    QLabel* idLabel = new QLabel(this);
    idLabel->setText("学号：");
    idLabel->move(20,lineEdit_bias_y+4+50);
    getId = new QLineEdit(this);
    getId->move(lineEdit_bias_x,lineEdit_bias_y+50);

    // 出生日期：  年    月    日
    QLabel* brithdayLabel = new QLabel(this);
    brithdayLabel->setText("出生日期：");
    brithdayLabel->move(20,lineEdit_bias_y+100+4);
    getBirthyear = new QLineEdit(this);
    getBirthyear->move(lineEdit_bias_x,lineEdit_bias_y+100);
    getBirthyear->setFixedWidth(40);
    QLabel* yearLabel = new QLabel(this);
    yearLabel->setText("年");
    yearLabel->move(lineEdit_bias_x+42,lineEdit_bias_y+100+4);
    getBirthmonth = new QLineEdit(this);
    getBirthmonth->move(lineEdit_bias_x+55,lineEdit_bias_y+100);
    getBirthmonth->setFixedWidth(20);
    QLabel* monthLabel = new QLabel(this);
    monthLabel->setText("月");
    monthLabel->move(lineEdit_bias_x+77,lineEdit_bias_y+100+4);
    getBirthday = new QLineEdit(this);
    getBirthday->move(lineEdit_bias_x+90,lineEdit_bias_y+100);
    getBirthday->setFixedWidth(20);
    QLabel* dayLabel = new QLabel(this);
    dayLabel->setText("日");
    dayLabel->move(lineEdit_bias_x+112,lineEdit_bias_y+100+4);

    // 出生地：    省     市      县
    QLabel* birthplaceLabel = new QLabel(this);
    birthplaceLabel->setText("出生地：");
    birthplaceLabel->move(20,lineEdit_bias_y+150+4);
    getBirthProvince = new QLineEdit(this);
    getBirthProvince->move(lineEdit_bias_x,lineEdit_bias_y+150);
    getBirthProvince->setFixedWidth(40);
    QLabel* birthProvinceLabel = new QLabel(this);
    birthProvinceLabel->setText("省");
    birthProvinceLabel->move(lineEdit_bias_x+42,lineEdit_bias_y+150+4);
    getBirthCity = new QLineEdit(this);
    getBirthCity->move(lineEdit_bias_x+55,lineEdit_bias_y+150);
    getBirthCity->setFixedWidth(40);
    QLabel* cityLabel = new QLabel(this);
    cityLabel->setText("市");
    cityLabel->move(lineEdit_bias_x+97,lineEdit_bias_y+150+4);
    getBirthCounty = new QLineEdit(this);
    getBirthCounty->move(lineEdit_bias_x+110,lineEdit_bias_y+150);
    getBirthCounty->setFixedWidth(40);
    QLabel* countyLabel = new QLabel(this);
    countyLabel->setText("县");
    countyLabel->move(lineEdit_bias_x+152,lineEdit_bias_y+150+4);
    return {lineEdit_bias_x+155+20, lineEdit_bias_y+150+4};
}

// 单例模式
StudentInfoInput* StudentInfoInput::getInstance()
{
    static StudentInfoInput ins;
    return &ins;
}


// @简介：槽函数，收到主界面按钮信号后打开信息录入界面
// @输入：无
// @输出：界面展示
void StudentInfoInput::showInput(){
    // 显示页面前清楚之前输入的信息
    getName->clear();
    getId->clear();
    getBirthyear->clear();
    getBirthmonth->clear();
    getBirthday->clear();
    getBirthProvince->clear();
    getBirthCity->clear();
    getBirthCounty->clear();
    this->show();
}

// @简介：槽函数，信息录入界面确认后传输信息
// @输入：无
// @输出：界面展示
void StudentInfoInput::tranInfo(){
    QString newInfo = getName->text()+"&"+getId->text()+"&"+getBirthyear->text()+"年"+getBirthmonth->text()+"月"+getBirthday->text()+"日"+"&"+getBirthProvince->text()+"省"
            +getBirthCity->text()+"市"+getBirthCounty->text()+"县（区）";
    qDebug() << newInfo;
    QNetworkReply* reply = client->send_msg_post(TcpClient::CREATE, newInfo);
    connect(reply, &QNetworkReply::finished, this, [=]() {
        bool ok;
        QByteArray data = reply->readAll();
        int reply_val = data.toInt(&ok, 10);
        qDebug() << reply_val;
        // 返回值 0 1 2 分别代表 失败 成功 重复操作
        if(reply_val == 0){
            msg->getMsgBox()->critical(this,"警告","档案建立失败，请重试！");
        }
        else if(reply_val == 1){
            msg->getMsgBox()->information(this,"提示","档案建立成功！");
        }else{
            msg->getMsgBox()->information(this,"提示","档案已存在！");
        }
    });
    //在使用QNetworkReply之后需要释放内存
    connect(reply, &QNetworkReply::finished, reply, &QObject::deleteLater);
    this->close();
}

StudentInfoInput::~StudentInfoInput()
{
    qDebug() << "~StudentInfoInput";
}
