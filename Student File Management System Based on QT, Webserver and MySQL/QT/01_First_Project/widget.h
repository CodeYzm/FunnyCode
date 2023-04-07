#ifndef WIDGET_H
#define WIDGET_H
#include<QApplication>
#include<QDebug>
#include <QWidget>
#include<QPushButton>
#include<QDialog>
#include<QShortcut>
#include"studentinfoinput.h"
#include"studentinfosearch.h"
#include "studentinfomodify.h"
#include"studentinfodelete.h"
#include"studentinfoclear.h"
#include"tcpclient.h"
#include"messagebox.h"

// 主界面，是程序的第二个界面，上面有新建、查找、修改、删除、清空五个选项
class Widget : public QWidget
{
    Q_OBJECT // 宏，允许使用信号和槽机制

public:
    Widget(QWidget *parent = 0);
    ~Widget();
    QPushButton* createButton(const QString buttonName, const QString iconPath, const int btnsizeH, const int btnGap);
private:
    StudentInfoInput* createDia;
    studentInfoSearch* searchDia;
    StudentInfoModify* modifyDia;
    StudentInfoDelete* deleteDia;
    StudentInfoClear* clearDia;
    MessageBox* msgBox;
    int buttonCount;
    int initx;
    int inity;

public slots:

};

#endif // WIDGET_H
