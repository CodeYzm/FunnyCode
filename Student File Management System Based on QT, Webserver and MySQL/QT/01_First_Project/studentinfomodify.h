#ifndef STUDENTINFOMODIFY_H
#define STUDENTINFOMODIFY_H

#include <QWidget>
#include<QLineEdit>
#include<QLabel>
#include<QPushButton>
#include<QDialog>
#include"messagebox.h"
#include"tcpclient.h"

// 修改学生档案的界面
class StudentInfoModify : public QWidget
{
    Q_OBJECT
public:
    static StudentInfoModify* getInstance();  // 使用单例模式
private:
    explicit StudentInfoModify(QWidget *parent = 0);
    ~StudentInfoModify();
    TcpClient* client;
    QLineEdit* studentId;
    QLabel* id;
    // 学生信息界面的显示
    QLabel* oldMsg;
    QLabel* newMsg;
    QLabel* getName;
    QLabel* getId;
    QLabel* getBirthday;
    QLabel* getBirthplace;
    QDialog *studentInfoDia;
    QLineEdit* newName;
    QLineEdit* newId;
    QLineEdit* newBirthday;
    QLineEdit* newBirthPlace;
    // 提示类
    MessageBox* msg;
    // 正则匹配检查，看输入是否符合数据格式要求
    bool birthday_check(QString birthday);
    bool birthplace_check(QString birthplace);
signals:

public slots:
    void dosearch(); // 先根据id搜索学生看是否存在
    void domodify(); // 修改学生信息
};

#endif // STUDENTINFOMODIFY_H
