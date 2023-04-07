#ifndef STUDENTINFOSEARCH_H
#define STUDENTINFOSEARCH_H

#include <QWidget>
#include<QLineEdit>
#include<QLabel>
#include<QPushButton>
#include<QDialog>
#include"messagebox.h"
#include"tcpclient.h"

// 查找学生档案界面
class studentInfoSearch : public QWidget
{
    Q_OBJECT
public:
    static studentInfoSearch* getInstance();
private:

    explicit studentInfoSearch(QWidget *parent = 0);  // 使用单例模式
    ~studentInfoSearch();
    TcpClient* client;
    QLineEdit* studentId;
    QLabel* id;
    // 学术信息界面的显示行
    QLabel* getName;
    QLabel* getId;
    QLabel* getBirthday;
    QLabel* getBirthplace;
    QDialog *studentInfoDia;
    // 提示类
    MessageBox* msg;
signals:

public slots:
    void dosearch(); // 搜索学生档案
};

#endif // STUDENTINFOSEARCH_H
