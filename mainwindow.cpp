#include "mainwindow.h"
#include "./ui_mainwindow.h"

// Вылетает при открытии новой папки после анализа
// Добавить функцию отсеивания файлов со 100% совпадением

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clearAll()
{
    engine.clear();
    filesList.clear();
    ui->listWidgetDup->clear();
    ui->listWidgetOrig->clear();
    clearOrig();
    clearDup();
}

void MainWindow::clearOrig()
{
    ui->labelOrigChangeDate->clear();
    ui->labelOrigSaveDate->clear();
    ui->labelOrigPath->clear();
    ui->labelOrigSize->clear();
    ui->labelOrigType->clear();
    ui->labelOrigExt->clear();
}

void MainWindow::clearDup()
{
    ui->labelDupType->clear();
    ui->labelDupChangeDate->clear();
    ui->labelDupSaveDate->clear();
    ui->labelDupExt->clear();
    ui->labelDupPath->clear();
    ui->labelDupSize->clear();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&engine, &CompareEngine::currentAction, this, &MainWindow::getAction);
}

void MainWindow::getAction(QString act)
{
    ui->textBrowserLog->append(act);
}

void MainWindow::on_pushButtonSourcePath_clicked()
{
    clearAll();
    QString path = QFileDialog::getExistingDirectory(this, "Выбор расположения файлов");
    if(!path.isEmpty())
    {
        ui->labelSourcePath->setText(path);
        engine.setPath(path);
    }
}

void MainWindow::on_pushButtonAnalize_clicked()
{
    if(engine.startCompare())
    {
        filesList = engine.getFilesList();
        for(auto i : filesList)
        {
            QListWidgetItem *item = new QListWidgetItem;
            item->setText(i[0].fileName());
            if(i.size() > 1)
            {
                item->setBackground(Qt::green);
            }
            ui->listWidgetOrig->addItem(item);
        }
    }
    else
    {
        QMessageBox::warning(this, "!", "Не указан путь к файлам!");
    }
}

void MainWindow::on_listWidgetOrig_currentRowChanged(int currentRow)
{
    ui->listWidgetDup->clear();
    int id = currentRow;
    QFileInfo &fileInfo = filesList[id][0];

    ui->labelOrigPath->setText(fileInfo.absoluteFilePath());
    ui->labelOrigChangeDate->setText(fileInfo.lastModified().toString());
    ui->labelOrigSaveDate->setText(fileInfo.birthTime().toString());
    ui->labelOrigSize->setText(QString::number(fileInfo.size()));
    ui->labelOrigType->setText(fileInfo.suffix());
    ui->labelOrigExt->setText(fileInfo.suffix());

    if(filesList[id].size() > 0)
    {
        for(int i = 1; i < filesList[id].size(); i++)
        {
            ui->listWidgetDup->addItem(filesList[id][i].fileName());
        }
    }

    QImage image(fileInfo.absoluteFilePath());
    QGraphicsScene* scene = new QGraphicsScene();
    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(image));
    scene->addItem(item);
    ui->graphicsViewOrig->setScene(scene);
    ui->graphicsViewOrig->fitInView(scene->sceneRect());
}

void MainWindow::on_listWidgetDup_currentRowChanged(int currentRow)
{
    int id = currentRow;
    QFileInfo &fileInfo = filesList[id][0];

    ui->labelDupPath->setText(fileInfo.absoluteFilePath());
    ui->labelDupChangeDate->setText(fileInfo.lastModified().toString());
    ui->labelDupSaveDate->setText(fileInfo.birthTime().toString());
    ui->labelDupSize->setText(QString::number(fileInfo.size()));
    ui->labelDupType->setText(fileInfo.suffix());
    ui->labelDupExt->setText(fileInfo.suffix());

    if(filesList[id].size() > 0)
    {
        for(int i = 1; i < filesList[id].size(); i++)
        {
            ui->listWidgetDup->addItem(filesList[id][i].fileName());
        }
    }
}

