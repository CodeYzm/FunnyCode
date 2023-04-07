#include "widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent), buttonCount(0)
{
    msgBox = MessageBox::getInstance();
    createDia = StudentInfoInput::getInstance();
    searchDia = studentInfoSearch::getInstance();
    modifyDia = StudentInfoModify::getInstance();
    deleteDia = StudentInfoDelete::getInstance();
    clearDia = StudentInfoClear::getInstance();
    // 整体按钮从上往下均匀分布，只有一列按钮
    int buttonNum = 5; // 按钮数量
    int winSizeW = 400, winSizeH = 400; // 窗口长宽
    this->setFixedSize(winSizeW ,winSizeH); // 固定窗口大小

    // 第一个按钮位置
    this->inity = 50, this->initx = winSizeW/2;
    // 计算按钮间隔
    int buttonGapH = (winSizeH - (this->inity * 2))/ buttonNum;


    // 创建管理系统主窗口按钮
    QPushButton* btnCreate = this->createButton("新建学生档案",":/firstPage/create.png", 30, buttonGapH);
    connect(btnCreate, &QPushButton::clicked,createDia,&StudentInfoInput::showInput);
    QPushButton* btnSearch = this->createButton("查找学生档案",":/firstPage/search.png", 30, buttonGapH);
    connect(btnSearch, &QPushButton::clicked,searchDia,&studentInfoSearch::show);
    QPushButton* btnModify = this->createButton("修改学生档案",":/firstPage/modify.png", 30, buttonGapH);
    connect(btnModify, &QPushButton::clicked,modifyDia,&StudentInfoModify::show);
    QPushButton* btnDelete = this->createButton("删除学生档案",":/firstPage/delete.png", 30, buttonGapH);
    connect(btnDelete, &QPushButton::clicked,deleteDia,&StudentInfoDelete::show);
    QPushButton* btnClear = this->createButton("清空学生档案",":/firstPage/clear.png", 30, buttonGapH);
    connect(btnClear, &QPushButton::clicked, clearDia, &StudentInfoClear::doclear);
}

// 创建按钮 输入：按钮名称 按钮图标路径 按钮高度 按钮间隔 输出：按钮对象
QPushButton* Widget::createButton(const QString buttonName, const QString iconPath, const int btnSizeH, const int btnGap)
{
    // 按钮
    QPushButton *btn = new QPushButton(buttonName, this);
    // 一个字大概15，再加上图标的宽度
    int btnSizeW = buttonName.size()*15 + btnSizeH;
    btn->resize(btnSizeW,btnSizeH);
    // 设置按钮位置
    btn->move(this->initx-btnSizeW/2, this->inity + this->buttonCount * btnGap);
    // 设置按钮图标
    QIcon icon(iconPath);
    btn->setIcon(icon);
    // 图标大小比按钮小10
    int iconSize = btnSizeH - 10;
    btn->setIconSize(QSize(iconSize,iconSize));
    // 增加当前窗口按钮计数
    ++this->buttonCount;
    return btn;
}

Widget::~Widget()
{
    qDebug() << "~widget";
//    delete this->cw;
}
