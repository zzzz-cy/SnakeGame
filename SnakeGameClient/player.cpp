#include "player.h"

Player::Player()
{

    //初始长度
    m_length=2;


    //初始移动方向  上
    m_x=0;
    m_y=-PLAYER_SPEED;

    //定义存在
    m_exist=true;

}

