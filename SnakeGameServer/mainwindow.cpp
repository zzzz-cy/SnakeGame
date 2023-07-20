#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //界面初始化
    ui->port->setText("9999");
    setWindowTitle("服务器");
    ui->disconnect->setEnabled(false);
    setWindowIcon(QIcon(":/res/snake.png"));


    m_status=new QLabel;
    m_status->setText("未连接");
    ui->statusBar->addWidget(m_status);

    //按下按键开始
    connect(ui->connect,&QPushButton::clicked,[=](){


        ui->connect->setDisabled(true);
        ui->disconnect->setEnabled(true);
        initServer();
        startServer();

    });
//设置断开连接按键
    connect(ui->disconnect,&QPushButton::clicked,[=](){

        ui->connect->setEnabled(true);
        ui->disconnect->setEnabled(false);

        //重置各数据
        foods.clear();

        m_status->setText("未连接");

        m_timer->stop();

        m_s->close();

        m_s->deleteLater();

    });
}

//服务器初始化
void MainWindow::initServer(){

    players.clear();
    m_tcp.clear();

    //新建server监听
    m_s=new QTcpServer(this);
    unsigned short port=ui->port->text().toUShort();
    m_s->listen(QHostAddress::Any,port);

    sendRecorder=0;
    foodRecorder=0;

    num=0;


}

//服务器启动
void MainWindow::startServer(){

    //启动计时器
    m_timer=new QTimer(this);
    m_timer->start();

    //客户端连接时新建Socket
    connect(m_s,&QTcpServer::newConnection,this,[=](){
        if(num>=PLAYER_MAX){
            return;
        }
        QTcpSocket *m_socket=m_s->nextPendingConnection();

        num++;

        //客户端断开时
        connect(m_socket,&QTcpSocket::disconnected,this,[=](){
            m_socket->close();
            m_socket->deleteLater();
            updateInfo();
            m_status->setText(QString("已连接"+QString::number(num)+"个客户端"));
        });


        //并连接好数据处理方法
        connect(m_socket,&QTcpSocket::readyRead,[=](){
            QString str = QString::fromUtf8(m_socket->readAll());
            ui->record->append("客户端say:"+str);
            dataProcess(str);
        });

        m_tcp.push_back(m_socket);

        m_status->setText(QString("已连接"+QString::number(num)+"个客户端"));

    });

    //连接计时器
    connect(m_timer,&QTimer::timeout,this,[=](){


        foodRecorder++;

        sendRecorder++;

        //定时更新食物
        if(foodRecorder>=FOOD_INTERVAL&&num>0){
            setFood();
            foodRecorder=0;
        }

        //定时刷新数据
        if(sendRecorder>=INTERVAL&&num>0){
            sendRecorder=0;
            updatePosition();
            if(players.size()==num){
                detect();
            }
            sendMessage();
        }
    });
}

//数据处理
void MainWindow::dataProcess(QString str){
    QStringList list=str.split("_");
    QStringListIterator *it=new QStringListIterator(list);
    while(it->hasNext()){
        QString s=it->next();
        if(s=="n"){
            //新加入玩家
            QString name=it->next();
            Player *player=new Player;
            player->m_name=name;
            int r=it->next().toInt();
            int g=it->next().toInt();
            int b=it->next().toInt();
            player->m_color=QColor(r,g,b,150);
            players.push_back(*player);
        }else if(s=="d"){
            //更新方向
            QString id=it->next();
            QVector<Player>::iterator it_p=players.begin();
            while(it_p!=players.end()){
                if(it_p->m_name==id){
                    it_p->m_x=it->next().toInt();
                    it_p->m_y=it->next().toInt();
                }
                it_p++;
            }
        }
    }
}

//排序
void MainWindow::sort(){
    //清空排名

    for(int i=0;i<5;i++){
        rank[i].m_name="_";
    }
    //进行排名

    QVector<Player>::iterator it_p=players.begin();
    while(it_p!=players.end()){
        if(it_p->m_exist) {
            for(int j=0;j<num&&j<5;j++){
                if(rank[j].m_name=="_"){
                    rank[j]=*it_p;
                    break;
                }
                if(rank[j].m_length<it_p->m_length){
                    for(int k=(num<5)?num-1:4;k>j;k--){
                        rank[k]=rank[k-1];
                    }
                    rank[j]=*it_p;
                    break;
                }
            }
        }
        it_p++;
    }
}

//发送信息
void MainWindow::sendMessage(){
    QString str;
    sort();
    str.append(QString("R_"+QString::number(num)+"_"));
    //传输排名信息
    for(int i=0;i<5;i++){
        if(rank[i].m_name=="_") break;
        str.append(rank[i].m_name+"_"+QString::number(rank[i].m_length)+"_");
    }
    //传输玩家信息
    str.append("S_"+QString::number(num)+"_");
    QVector<Player>::iterator it_p=players.begin();
    while(it_p!=players.end()){
        str.append(it_p->m_name+"_"+QString::number(it_p->m_exist)+"_"+QString::number(it_p->m_length)+"_"+QString::number(it_p->m_color.red())+"_"+QString::number(it_p->m_color.green())+"_"+QString::number(it_p->m_color.blue())+"_");
        QVector<QRect>::iterator it_b=it_p->m_snake.begin();
        for(int i=0;i<it_p->m_length;i++){
            str.append(QString::number(it_b->x())+"_"+QString::number(it_b->y())+"_");
            it_b++;
        }
        it_p++;
    }
    //传输食物信息
    str.append("F_"+QString::number(foods.size())+"_");
    QVector<QRect>::iterator it_f=foods.begin();
    while (it_f!=foods.end()) {
        str.append(QString::number(it_f->x())+"_"+QString::number(it_f->y())+"_");
        it_f++;
    }
    ui->record->append("服务器say:"+str);
    //逐个发送
    for(int i=0;i<num;i++){
        m_tcp[i]->write(str.toUtf8());
    }
}

//检测
void MainWindow::detect(){
    QVector<Player>::iterator it_p=players.begin();
    while(it_p!=players.end()){
        if(it_p->m_exist){
            //玩家碰撞检测
            QVector<Player>::iterator it_p2=players.begin();
            while(it_p2!=players.end()){
                if(it_p!=it_p2 && it_p2->m_exist){
                    QVector<QRect>::iterator it_body=it_p2->m_snake.begin();
                    while(it_body!=it_p2->m_snake.end()){
                        if(it_p->m_snake[0].intersects(*it_body)){
                            it_p->m_exist=false;
                            setFoodFromSnake(*it_p);
                            break;
                        }
                        it_body++;
                    }
                }

                it_p2++;
                if(!it_p->m_exist)break;
            }
        }
        if(it_p->m_exist){
            //进食检测
            QVector<QRect>::Iterator it_f=foods.begin();
            while(it_f!=foods.end()){
                if(it_p->m_snake.at(0).intersects(*it_f)){
                    foods.removeAt(it_f-foods.begin());
                    it_p->eat();
                    break;
                }
                it_f++;
            }
        }
        it_p++;
    }
updateInfo();
}

//间隔一定时间生成食物
void MainWindow::setFood(){
    QRect rect;
    rect.setX(rand()%(GAME_WIDTH-FOOD_WIDTH));
    rect.setY(rand()%(GAME_HEIGHT-FOOD_HEIGHT));
    rect.setSize(QSize(FOOD_WIDTH,FOOD_WIDTH));
    foods.push_back(rect);
}

//删除断联玩家
void MainWindow::updateInfo(){

    for(int i=0;i<num;i++){
        if(!m_tcp[i]->isOpen()){
            players.removeAt(i);
            m_tcp.removeAt(i);
            num--;
            if(num==0){
                emit ui->disconnect->click();
            }
            m_status->setText(QString("已连接"+QString::number(num)+"个客户端"));
            return;
        }

        if(!players[i].m_exist){
            num--;
            QString deadInfo=QString("Dead_"+QString::number(players[i].m_length)+"_");
            m_tcp[i]->write(deadInfo.toUtf8());
            players.removeAt(i);
            m_tcp.removeAt(i);
            m_status->setText(QString("已连接"+QString::number(num)+"个客户端"));
            if(num==0){
                emit ui->disconnect->click();
            }
            return;
        }
    }


}

void MainWindow::updatePosition(){
    //蛇的前进
    QVector<Player>::iterator it_p=players.begin();
    while (it_p!=players.end()) {
        if(it_p->m_exist){
            QVector<QRect>::iterator it_b=it_p->m_snake.end()-1;
            while (it_b!=it_p->m_snake.begin()) {
                *it_b=*(it_b-1);
                it_b--;
            }
            it_b->moveTo(it_b->x()+it_p->m_x,it_b->y()+it_p->m_y);
        }
        if(it_p->m_snake[0].x()<=0||it_p->m_snake[0].x()>=GAME_WIDTH-SNAKE_WIDTH){
            it_p->m_x=-it_p->m_x;
        }
        if(it_p->m_snake[0].y()<=0||it_p->m_snake[0].y()>=GAME_WIDTH-SNAKE_WIDTH){
            it_p->m_y=-it_p->m_y;
        }
        it_p++;
    }
}

//死亡时生成食物
void MainWindow::setFoodFromSnake(Player player){
    //将身体每一节变成食物
    for(QRect body:player.m_snake){
        QRect food;
        food.setSize(QSize(FOOD_WIDTH,FOOD_WIDTH));
        food.moveTo(body.x(),body.y());
        foods.push_back(food);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
