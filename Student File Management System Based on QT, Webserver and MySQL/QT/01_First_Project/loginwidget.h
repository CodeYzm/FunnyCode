#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H


#include"widget.h"

// 系统登录窗口类，是程序的第一个界面
class LoginWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LoginWidget(QWidget *parent = 0);
    ~LoginWidget();
private:
    TcpClient* clientTcp;
    QLineEdit* account;
    QLineEdit* password;
    QWidget* registerWid; // 注册界面
    MessageBox* registerMsg;
    QLineEdit* newAccount;
    QLineEdit* newPassword;
    QWidget* mainWid; // 主界面
signals:

public slots:
    void onRegisterButtonClicked();
    void onLoginButtonClicked();
    void onSignupButtonClicked();
};

#endif // LOGINWIDGET_H
