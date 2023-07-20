#include "mainscene.h"
#include "ui_mainscene.h"





MainScene::MainScene(QTcpSocket *tcp,QString name,QColor color,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainScene)
{
    ui->setupUi(this);

    //接收登录信息
    m_tcp=tcp;
    m_username=name;
    m_color=color;

    m_x=0;
    m_y=PLAYER_SPEED;
    

    //初始化并开始游戏
    initScene();
    startGame();

}

//初始化设置
void MainScene::initScene()
{
    setWindowModality(Qt::ApplicationModal);//模态化
    setWindowTitle("贪吃蛇");
    setFixedSize(GAME_WIDTH,GAME_HEIGHT);
    setWindowIcon(QIcon(":/res/snake.png"));
}
void MainScene::startGame(){


    //初次连接传输新建玩家的指令
    QString str=QString("n_"+m_username+"_"+QString::number(m_color.red())+"_"+QString::number(m_color.green())+"_"+QString::number(m_color.blue())+"_");
    m_tcp->write(str.toUtf8());

    //读取服务器传输的数据
    connect(m_tcp,&QTcpSocket::readyRead,[=](){
        //更新各种数据
        QString str2=QString::fromUtf8(m_tcp->readAll());
        //调用数据处理函数
        this->dataProcess(str2);
        update();//重绘窗口
    });
    connect(m_tcp,&QTcpSocket::disconnected,[=](){
        QMessageBox::warning(this,"连接错误","已和服务器断开连接");
        m_tcp->close();
        m_tcp->deleteLater();
        LogIn *li=new LogIn();
        this->close();
        li->show();
    });
}

//数据读取函数
void MainScene::dataProcess(QString str){
    QStringList list=str.split("_");
    QStringListIterator *it=new QStringListIterator(list);
    while(it->hasNext()){
        QString s=it->next();
        if(s=="S"){
            //接收蛇的全部信息
            players.clear();
            int num=it->next().toInt();
            for(int i=0;i<num;i++){
                Player *player=new Player();
                player->m_name=it->next();
                player->m_exist=it->next().toInt();
                player->m_length=it->next().toInt();
                int r=it->next().toInt();
                int g=it->next().toInt();
                int b=it->next().toInt();
                player->m_color=QColor(r,g,b,150);
                for(int i=0;i<player->m_length;i++){
                    QRect rect;
                    rect.setX(it->next().toInt());
                    rect.setY(it->next().toInt());
                    rect.setWidth(SNAKE_WIDTH);
                    rect.setHeight(SNAKE_WIDTH);
                    player->m_snake.push_back(rect);
                }
                players.push_back(*player);
            }
        }else if(s=="F"){
            foods.clear();
            int num=it->next().toInt();
            for(int i=0;i<num;i++){
                QRect food;
                food.setX(it->next().toInt());
                food.setY(it->next().toInt());
                food.setSize(QSize(FOOD_WIDTH,FOOD_HEIGHT));
                foods.push_back(food);
            }
        }else if(s=="R"){
            int num=it->next().toInt();
            ui->rank->setText(QString("当前共有%1名玩家").arg(num));
            for(int i=0;i<num && i<5;i++){
                QString str2=it->next();
                ui->rank->append(QString::number(i+1)+". "+str2+"  "+it->next()+"分");
            }
        }else if(s=="Dead"){
            QString score=QString("你的最终得分为"+it->next()+"分");
            QMessageBox::about(this,"Game Over",score);
            m_tcp->close();
            m_tcp->deleteLater();
            this->close();
        }

    }
}

//方向操作
void MainScene::keyPressEvent(QKeyEvent *e){
    int x=m_x;
    int y=m_y;
    if(e->key()==Qt::Key_A){
        x=-PLAYER_SPEED;
        y=0;
    }
    if(e->key()==Qt::Key_W){
        y=-PLAYER_SPEED;
        x=0;
    }
    if(e->key()==Qt::Key_S){
        y=PLAYER_SPEED;
        x=0;
    }
    if(e->key()==Qt::Key_D){
        x=PLAYER_SPEED;
        y=0;
    }
    //方向发生改变时发送信息

    if(x!=m_x||y!=m_y){
        m_x=x;
        m_y=y;
        QString str=QString("d_"+m_username+"_"+QString::number(m_x)+"_"+QString::number(m_y)+"_");
        m_tcp->write(str.toUtf8());
    }

}

//绘制
void MainScene::paintEvent(QPaintEvent *e){
    QPainter painter(this);

    //绘制蛇
    QVector<Player>::iterator it1=players.begin();
    while (it1!=players.end()) {
        if(it1->m_exist){
            QVector<QRect>::iterator it2=it1->m_snake.begin();
            while(it2!=it1->m_snake.end()){
                painter.fillRect(it2->x(),it2->y(),it2->width(),it2->height(),it1->m_color);
                it2++;
            }
        }
        it1++;
    }
    //绘制食物
    QVector<QRect>::iterator it_f=foods.begin();
    while (it_f!=foods.end()) {
        painter.drawRect(*it_f);
        it_f++;
    }
}


MainScene::~MainScene()
{
    delete ui;
}
