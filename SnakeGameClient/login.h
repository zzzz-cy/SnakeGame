#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QMessageBox>
#include <mainscene.h>
#include <QTcpSocket>
#include <QColorDialog>

namespace Ui {
class LogIn;
}

class LogIn : public QWidget
{
    Q_OBJECT

public:
    explicit LogIn(QWidget *parent = 0);
    ~LogIn();

    QColor color;

private:
    Ui::LogIn *ui;
};

#endif // LOGIN_H
