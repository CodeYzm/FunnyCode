#include "messagebox.h"
#include<QDebug>
MessageBox::MessageBox(QObject *parent) :
    QObject(parent)
{
    QMsgBox = new QMessageBox();
}

QMessageBox* MessageBox::getMsgBox()
{
    return QMsgBox;
}

MessageBox* MessageBox::getInstance()
{
    static MessageBox msgBox;
    return &msgBox;
}

MessageBox::~MessageBox()
{
    delete QMsgBox;
    qDebug() << "~MessageBox";
}
