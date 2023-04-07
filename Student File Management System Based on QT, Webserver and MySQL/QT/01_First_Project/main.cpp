#include "loginwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    // 应用程序对象，有且仅有一个
    QApplication app(argc, argv);
    // 窗口对象
    LoginWidget w;
    w.show();


    // 让对象进入消息循环
    return app.exec();
}
