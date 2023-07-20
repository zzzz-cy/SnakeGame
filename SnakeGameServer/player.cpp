#include "player.h"


Player::Player(){

    //初始长度
    m_length=2;

    QRect rect1;
    rect1.setX(rand()%(GAME_WIDTH-2)+1);
    rect1.setY(rand()%(GAME_HEIGHT-3)+1);
    rect1.setWidth(SNAKE_WIDTH);
    rect1.setHeight(SNAKE_WIDTH);

    QRect rect2;
    rect2.setX(rect1.x());
    rect2.setY(rect2.y()+SNAKE_WIDTH+1);
    rect2.setSize(QSize(SNAKE_WIDTH,SNAKE_WIDTH));

    m_snake.push_back(rect1);
    m_snake.push_back(rect2);

    //初始移动方向  上
    m_x=0;
    m_y=-PLAYER_SPEED;

    //定义存在
    m_exist=true;
}


//定义吃的动作  沿着尾巴方向加长
void Player::eat(){

    if(m_snake[m_length-1].x()==m_snake[m_length-2].x()){
        QRect rect;
        rect.setX(m_snake[m_length-1].x());
        rect.setY(m_snake[m_length-1].y()+SNAKE_WIDTH+1);
        rect.setSize(QSize(SNAKE_WIDTH,SNAKE_WIDTH));
        m_snake.push_back(rect);
        m_length++;
    }
    if(m_snake[m_length-1].y()==m_snake[m_length-2].y()){
        QRect rect;
        rect.setX(m_snake[m_length-1].x()+SNAKE_WIDTH+1);
        rect.setY(m_snake[m_length-1].y());
        rect.setSize(QSize(SNAKE_WIDTH,SNAKE_WIDTH));
        m_snake.push_back(rect);
        m_length++;
    }
}
