#ifndef MAINSCENE_H
#define MAINSCENE_H

#include <QMainWindow>
#include <player.h>
#include <config.h>
#include <QPixmap>
#include <QTimer>
#include <QTcpSocket>
#include <QVector>
#include <QKeyEvent>
#include <QByteArray>
#include <QHostAddress>
#include <QtDebug>
#include <QPainter>
#include <QStringListIterator>
#include <QVectorIterator>
#include <QColor>
#include <QMessageBox>
#include <login.h>

namespace Ui {
class MainScene;
}

class MainScene : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainScene(QTcpSocket *tcp,QString name,QColor color,QWidget *parent = 0);
    ~MainScene();

    //接收登录信息
    QTcpSocket *m_tcp;
    QString m_username;
    QColor m_color;

    //定义玩家对象
    QVector<Player> players;

    //定义食物
    QVector<QRect> foods;

    //更新方向
    int m_x;
    int m_y;


    //界面初始化
    void initScene();

    //解析服务器传来的数据
    void read();

    //定义游戏启动
    void startGame();

    //键盘事件
    void keyPressEvent(QKeyEvent *e);

    //绘制
    void paintEvent(QPaintEvent *e);

    //数据处理函数
    void dataProcess(QString str);   



private:
    Ui::MainScene *ui;
};

#endif // MAINSCENE_H
