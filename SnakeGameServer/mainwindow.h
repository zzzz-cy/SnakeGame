#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
#include <QTcpServer>
#include <mysocket.h>
#include <QString>
#include <QVectorIterator>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    //定义排名
    Player rank[5];

    //玩家人数
    int num;

    //计时器
    QTimer *m_timer;

    //数据传输计时器
    int sendRecorder;

    //食物生成计时器
    int foodRecorder;

    //网络
    QTcpServer *m_s;

    //玩家
    QVector<Player> players;

    //定义食物
    QVector<QRect> foods;


    //定义监听数组
    QVector<QTcpSocket*> m_tcp;

    //界面
    QLabel *m_status;

    //服务器初始化方法
    void initServer();

    //服务器启动方法
    void startServer();

    //定义数据处理方法
    void dataProcess(QString str);

    //信息传输方法
    void sendMessage();

    //判断函数
    void detect();

    //更新位置函数
    void updatePosition();

    //更新信息
    void updateInfo();

    //排序
    void sort();

    //生成食物
    void setFood();

    //死亡生成食物
    void setFoodFromSnake(Player player);



private:
    Ui::MainWindow *ui;





};

#endif // MAINWINDOW_H
