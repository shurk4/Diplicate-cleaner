#include "mainmaster.h"
#include "ui_mainmaster.h"

MainMaster::MainMaster(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainMaster)
{
    ui->setupUi(this);
//    ui->tabWidget.setT
}

MainMaster::~MainMaster()
{
    delete ui;
}
