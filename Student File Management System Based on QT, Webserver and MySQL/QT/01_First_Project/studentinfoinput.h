#ifndef STUDENTINFOINPUT_H
#define STUDENTINFOINPUT_H
#include<QLineEdit>
#include<QLabel>
#include <QWidget>
#include "tcpclient.h"
#include "messagebox.h"

// 新建学生档案的界面
class StudentInfoInput : public QWidget
{
    Q_OBJECT
public:
    static StudentInfoInput* getInstance(); // 单例模式
private:
    explicit StudentInfoInput(QWidget *parent = 0);
    std::pair<int,int> initLineEditPage();
    ~StudentInfoInput();
    // 学术信息界面的输入行
    QLineEdit* getName;
    QLineEdit* getId;
    QLineEdit* getBirthyear;
    QLineEdit* getBirthmonth;
    QLineEdit* getBirthday;
    QLineEdit* getBirthProvince;
    QLineEdit* getBirthCity;
    QLineEdit* getBirthCounty;
    // 通信类
    TcpClient* client;
    // 提示类
    MessageBox* msg;
signals:

public slots:
    void showInput();
    void tranInfo();
};

#endif // STUDENTINFOINPUT_H
