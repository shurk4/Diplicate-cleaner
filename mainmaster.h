#ifndef MAINMASTER_H
#define MAINMASTER_H

#include <QMainWindow>

namespace Ui {
class MainMaster;
}

class MainMaster : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainMaster(QWidget *parent = nullptr);
    ~MainMaster();

private:
    Ui::MainMaster *ui;
};

#endif // MAINMASTER_H
