#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QObject>
#include<QMessageBox>

// 提示类，用于利用QMessageBox生成各种提示窗口
class MessageBox : public QObject
{
    Q_OBJECT
public:
    static MessageBox* getInstance(); // 使用单例模式
    QMessageBox* getMsgBox();
private:
    explicit MessageBox(QObject *parent = 0);
    ~MessageBox();
    QMessageBox* QMsgBox;
signals:

public slots:

};

#endif // MESSAGEBOX_H
