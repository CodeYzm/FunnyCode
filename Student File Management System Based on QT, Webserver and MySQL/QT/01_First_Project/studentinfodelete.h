#ifndef STUDENTINFODELETE_H
#define STUDENTINFODELETE_H

#include <QWidget>
#include<QLineEdit>
#include<QLabel>
#include<QPushButton>
#include<QDialog>
#include"messagebox.h"
#include"tcpclient.h"

// 删除学生档案的界面
class StudentInfoDelete : public QWidget
{
    Q_OBJECT
public:
    static StudentInfoDelete* getInstance();  // 使用单例模式
private:
    explicit StudentInfoDelete(QWidget *parent = 0);
    ~StudentInfoDelete();
    TcpClient* client;
    QLineEdit* studentId;
    QLabel* id;
    // 提示类
    MessageBox* msg;

signals:

public slots:
    void dodelete();
};

#endif // STUDENTINFODELETE_H
