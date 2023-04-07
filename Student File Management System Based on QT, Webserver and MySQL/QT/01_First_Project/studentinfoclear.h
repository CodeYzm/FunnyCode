#ifndef STUDENTINFOCLEAR_H
#define STUDENTINFOCLEAR_H

#include <QWidget>
#include<QLineEdit>
#include<QLabel>
#include<QPushButton>
#include<QDialog>
#include"messagebox.h"
#include"tcpclient.h"

// 清空数据库的界面
class StudentInfoClear : public QWidget
{
    Q_OBJECT
public:
    static StudentInfoClear* getInstance();  // 使用单例模式
private:
    explicit StudentInfoClear(QWidget *parent = 0);
    ~StudentInfoClear();
    TcpClient* client;
    // 提示类
    MessageBox* msg;

signals:

public slots:
    void doclear();
};

#endif // STUDENTINFOCLEAR_H
