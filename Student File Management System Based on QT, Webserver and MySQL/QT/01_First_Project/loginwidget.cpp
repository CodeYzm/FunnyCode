#include "loginwidget.h"

LoginWidget::LoginWidget(QWidget *parent) :
    QWidget(parent)
{
    this->setFocusPolicy(Qt::StrongFocus);// 设置焦点策略为接受焦点
    mainWid = new Widget();
    clientTcp = TcpClient::getSocket(); // 登录窗口初始化服务器连接

    // 登录界面设计
    this->setFixedSize(280,200);
    QLabel* acLabel = new QLabel("账号",this);
    QLabel* pwdLabel = new QLabel("密码",this);
    acLabel->move(50,54);
    pwdLabel->move(50,104);
    account = new QLineEdit(this);
    password = new QLineEdit(this);
    account->move(80,50);
    password->move(80,100);
    // 登录窗口按钮设置
    QPushButton *loginBtn = new QPushButton("登录",this);
    loginBtn->move(50,140);
    loginBtn->setDefault(true);
    loginBtn->setFocus();


    QPushButton *registerBtn = new QPushButton("注册",this);
    registerBtn->move(150,140);

    // 登录和注册按钮对应的槽函数
    connect(loginBtn, SIGNAL(clicked()), this, SLOT(onLoginButtonClicked()));
    connect(registerBtn, SIGNAL(clicked()), this, SLOT(onRegisterButtonClicked()));

    // 注册界面设计
    registerMsg = MessageBox::getInstance();
    registerWid = new QWidget();
    QLabel* newAcLabel = new QLabel("账号",registerWid);
    QLabel* newPwdLabel = new QLabel("密码",registerWid);
    newAcLabel->move(50,54);
    newPwdLabel->move(50,104);
    newAccount = new QLineEdit(registerWid);
    newPassword = new QLineEdit(registerWid);
    newAccount->move(80,50);
    newPassword->move(80,100);
    QPushButton *signupBtn = new QPushButton("注册",registerWid);
    signupBtn->move(100,140);
    // 注册页面注册按钮对应的槽函数
    connect(signupBtn, SIGNAL(clicked()), this, SLOT(onSignupButtonClicked()));
}

// 跳转到注册界面
void LoginWidget::onRegisterButtonClicked()
{
    registerWid->show();
}

void LoginWidget::onSignupButtonClicked()
{
   // 执行注册操作
    QString user = newAccount->text();
    QString pwd = newPassword->text();
    QString msg = "user=" + user + "&" + "password=" + pwd;

    // 向服务器发送post请求传送注册的用户名密码
    QNetworkReply* reply = clientTcp->send_msg_post(TcpClient::REGISTER,msg);
    connect(reply, &QNetworkReply::finished, this, [=]() {
        bool ok;
        QByteArray data = reply->readAll();
        int reply_val = data.toInt(&ok, 10);
        // 返回值 0 1 2 分别代表 失败 成功 重复操作
        if(reply_val == 0){
            registerMsg->getMsgBox()->critical(this,"警告","注册失败！");
        }
        else if(reply_val == 1){
            registerMsg->getMsgBox()->information(this,"提示","注册成功！");
        }else{
            registerMsg->getMsgBox()->information(this,"提示","该账号已注册！");
        }

    });

    // 在使用QNetworkReply之后需要释放内存
    connect(reply, &QNetworkReply::finished, reply, &QObject::deleteLater);
    // 关闭注册窗口
    registerMsg->getMsgBox()->close();
}

void LoginWidget::onLoginButtonClicked()
{
   // 执行登录操作
    QString user = account->text();
    QString pwd = password->text();
    QString msg = "user=" + user + "&" + "password=" + pwd;
    QNetworkReply* reply = clientTcp->send_msg_post(TcpClient::LOGIN,msg);
    connect(reply, &QNetworkReply::finished, this, [=]() {
        bool ok;
        QByteArray data = reply->readAll();
        int reply_val = data.toInt(&ok, 10);
        // 返回值 0 1 2 分别代表 失败 成功 重复操作
        if(reply_val == 0){
            registerMsg->getMsgBox()->critical(this,"警告","登录失败，请重试！");
        }
        else if(reply_val == 1){
            registerMsg->getMsgBox()->information(this,"提示","登录成功！");
            // 登录成功则关闭登录界面，打开主界面
            mainWid->show();
            this->close();

        }else{
            registerMsg->getMsgBox()->critical(this,"警告","服务器异常！");
        }
    });
    // 在使用QNetworkReply之后需要释放内存
    connect(reply, &QNetworkReply::finished, reply, &QObject::deleteLater);
}



LoginWidget::~LoginWidget()
{
    qDebug() << "~LoginWidget";
    delete mainWid;
    delete registerWid;
}
