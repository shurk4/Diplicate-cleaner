#include "mainwindow.h"
#include "./ui_mainwindow.h"

// Разобраться с ресайзом
// Добавить функцию отсеивания файлов со 100% совпадением

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&engine, &CompareEngine::currentAction, this, &MainWindow::getAction);
    origScene = new QGraphicsScene();
    ui->graphicsViewOrig->setScene(origScene);
    dupScene = new QGraphicsScene();
    ui->graphicsViewDup->setScene(dupScene);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clearAll()
{
    engine.clear();
    filesList.clear();

    ui->textBrowserLog->clear();

    clearOrig();
    clearDup();
}

void MainWindow::clearOrig()
{
    ui->graphicsViewOrig->scene()->clear();

    ui->listWidgetOrig->blockSignals(true);
    ui->listWidgetOrig->clear();
    ui->listWidgetOrig->blockSignals(false);

    ui->labelOrigChangeDate->clear();
    ui->labelOrigSaveDate->clear();
    ui->labelOrigPath->clear();
    ui->labelOrigSize->clear();
    ui->labelOrigType->clear();
    ui->labelOrigExt->clear();
}

void MainWindow::clearDup()
{
    ui->graphicsViewDup->scene()->clear();

    ui->listWidgetDup->blockSignals(true);
    ui->listWidgetDup->clear();
    ui->listWidgetDup->blockSignals(false);

    ui->labelDupType->clear();
    ui->labelDupChangeDate->clear();
    ui->labelDupSaveDate->clear();
    ui->labelDupExt->clear();
    ui->labelDupPath->clear();
    ui->labelDupSize->clear();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    ui->graphicsViewOrig->scene()->clear();
    showImage(ui->labelOrigPath->text(), ui->graphicsViewOrig, origScene);
    ui->graphicsViewDup->scene()->clear();
    showImage(ui->labelDupPath->text(), ui->graphicsViewDup, dupScene);
    QWidget::resizeEvent(event);
}

void MainWindow::getAction(QString act)
{
    ui->textBrowserLog->append(act);
}

void MainWindow::on_pushButtonRotateLeft_clicked()
{
    ui->graphicsViewOrig->rotate(-90);
    ui->graphicsViewDup->rotate(-90);
}

void MainWindow::on_pushButtonRotateRight_clicked()
{
    ui->graphicsViewOrig->rotate(90);
    ui->graphicsViewDup->rotate(90);
}

void MainWindow::showImage(const QString &path, QGraphicsView *view, QGraphicsScene *scene)
{
    QImage image(path);
    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(image.scaled(view->size(), Qt::KeepAspectRatio)));
    scene->addItem(item);
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
            if(i.size() == 2)
            {
                item->setBackground(Qt::green);
            }
            else if(i.size() == 3)
            {
                item->setBackground(Qt::yellow);
            }
            else if(i.size() > 3)
            {
                item->setBackground(Qt::red);
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
    clearDup();
    selectedFileId = currentRow; // сделать глобально
    QFileInfo fileInfo = filesList[selectedFileId][0];

    ui->labelOrigPath->setText(fileInfo.absoluteFilePath());
    ui->labelOrigChangeDate->setText(fileInfo.lastModified().toString());
    ui->labelOrigSaveDate->setText(fileInfo.birthTime().toString());
    ui->labelOrigSize->setText(QString::number(fileInfo.size()));
    ui->labelOrigType->setText(fileInfo.suffix());
    ui->labelOrigExt->setText(fileInfo.suffix());

    if(filesList[selectedFileId].size() > 1)
    {
        for(int i = 1; i < filesList[selectedFileId].size(); i++)
        {
            ui->listWidgetDup->addItem(filesList[selectedFileId][i].fileName());
        }
    }

    showImage(fileInfo.absoluteFilePath(), ui->graphicsViewOrig, origScene);
}

void MainWindow::on_listWidgetDup_currentRowChanged(int currentRow)
{
    selectedDupFileId = currentRow + 1;
    QFileInfo fileInfo = filesList[selectedFileId][selectedDupFileId];

    ui->labelDupPath->setText(fileInfo.absoluteFilePath());
    ui->labelDupChangeDate->setText(fileInfo.lastModified().toString());
    ui->labelDupSaveDate->setText(fileInfo.birthTime().toString());
    ui->labelDupSize->setText(QString::number(fileInfo.size()));
    ui->labelDupType->setText(fileInfo.suffix());
    ui->labelDupExt->setText(fileInfo.suffix());

    showImage(fileInfo.absoluteFilePath(), ui->graphicsViewDup, dupScene);
}
