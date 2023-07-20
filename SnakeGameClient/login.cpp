#include "login.h"
#include "ui_login.h"

LogIn::LogIn(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogIn)
{
    ui->setupUi(this);

    //界面的初始化
    //组件添加由ui完成
    this->setWindowTitle("贪吃蛇 登录");
    color=QColor(0,0,0,150);
    setWindowIcon(QIcon(":/res/snake.png"));

    //实现颜色选择
    connect(ui->btn_color,&QPushButton::clicked,[=](){
        QColorDialog *colorDialog=new QColorDialog(this);
        colorDialog->setOption(QColorDialog::ShowAlphaChannel);
        connect(colorDialog,&QColorDialog::finished,[=](){
            color=colorDialog->currentColor();
            qDebug()<<color;
        });
        colorDialog->exec();

    });

    //实现登录按钮
    connect(ui->btn_login,&QPushButton::clicked,[=](){
        QString username=ui->username->text();
        if(ui->port->text()==""||ui->username->text()==""||ui->ip->text()==""){
            ui->warning->setText("请输入相关信息");
        }else if(username.contains("_")){
            ui->warning->setText("用户名内不可以出现\"_\"");
        }else{
            //检测能否连接
            QTcpSocket *tcp=new QTcpSocket();
            //成功连接则跳转游戏界面
            connect(tcp,&QTcpSocket::connected,[=](){
                MainScene *ms=new MainScene(tcp,ui->username->text(),color);
                this->close();
                ms->show();
            });
            tcp->connectToHost(QHostAddress(ui->ip->text()),ui->port->text().toUShort());
            ui->warning->setText("连接失败，请输入可用的IP和端口");

        }
    });

    //实现退出按钮
    connect(ui->btn_exit,&QPushButton::clicked,this,&LogIn::close);

}

LogIn::~LogIn()
{
    delete ui;
}
