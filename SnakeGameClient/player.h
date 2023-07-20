#ifndef PLAYER_H
#define PLAYER_H
#include <QString>
#include <QRect>
#include <config.h>
#include <QVector>
#include <QColor>

class Player
{
public:
    Player();

    QColor m_color;

    //玩家名
    QString m_name;

    //蛇
    QVector<QRect> m_snake;

    //长度
    int m_length;

    //死亡判断
    bool m_exist;

    //定义行动方向
    int m_x;
    int m_y;



};

#endif // PLAYER_H
