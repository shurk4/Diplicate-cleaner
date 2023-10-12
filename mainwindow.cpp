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


    engine.moveToThread(&thread);
    connect(this, &MainWindow::runCompare, &engine, &CompareEngine::runCompare);
    connect(&engine, &CompareEngine::finishedCompare, this, &MainWindow::finishedCompare);
    connect(this, &MainWindow::runFull, &engine, &CompareEngine::runFullCompare);
    connect(&engine, &CompareEngine::finishedFull, this, &MainWindow::finishedFull);
    connect(this, &MainWindow::runDelete, &engine, &CompareEngine::runDelete);
    connect(&engine, &CompareEngine::finishedDelete, this, &MainWindow::finishedDelete);
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

void MainWindow::clearCounters()
{
    filesWithDup = 0;
    files2Dup = 0;
    files3Dup = 0;
    filesMoreDup = 0;
    ui->labelFilesNum->clear();
    ui->labelFilesWithDup->clear();
    ui->label2Dup->clear();
    ui->label3Dup->clear();
    ui->labelMoreDup->clear();
}

void MainWindow::showFilesList()
{
    clearOrig();
    clearDup();

    int filesNum = engine.getOrigNum();

    for(int i = 0; i < filesNum; i++)
    {
        QListWidgetItem *item = new QListWidgetItem;
        item->setText(engine.getOrigName(i));

        int dupNum = engine.getDupNum(i);
        if(dupNum > 1)
        {
            filesWithDup++;
        }
        if(dupNum == 2)
        {
            files2Dup++;
            item->setBackground(Qt::green);
        }
        else if(dupNum == 3)
        {
            files3Dup++;
            item->setBackground(Qt::yellow);
        }
        else if(dupNum > 3)
        {
            filesMoreDup++;
            item->setBackground(Qt::red);
        }
        ui->listWidgetOrig->addItem(item);
        ui->labelFilesNum->setText(QString::number(engine.getScannedFilesNum()));
        ui->labelFilesWithDup->setText(QString::number(filesWithDup));
        ui->label2Dup->setText(QString::number(files2Dup));
        ui->label3Dup->setText(QString::number(files3Dup));
        ui->labelMoreDup->setText(QString::number(filesMoreDup));
    }
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

void MainWindow::finishedCompare()
{
    showFilesList();
    thread.terminate();
}

void MainWindow::finishedFull()
{
    showFilesList();
    thread.terminate();
}

void MainWindow::finishedDelete()
{

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
        sourcePath = path;
        engine.setPath(path);
    }
}

void MainWindow::on_pushButtonAnalize_clicked()
{
    qDebug() << "MainWindow thread: " << QThread::currentThreadId();
    if(sourcePath.isEmpty())
    {
        QMessageBox::warning(this, "!", "Не указан путь к файлам!");
        return;
    }

    thread.start();
    emit runCompare();
//    showFilesList();
}

void MainWindow::on_listWidgetOrig_currentRowChanged(int currentRow)
{
    clearDup();
    selectedFileId = currentRow; // сделать глобально
    QFileInfo fileInfo = engine.getOrigInfo(currentRow);

    ui->labelOrigPath->setText(fileInfo.absoluteFilePath());
    ui->labelOrigChangeDate->setText(fileInfo.lastModified().toString());
    ui->labelOrigSaveDate->setText(fileInfo.birthTime().toString());
    ui->labelOrigSize->setText(QString::number(fileInfo.size()));
    ui->labelOrigType->setText(fileInfo.suffix());
    ui->labelOrigExt->setText(fileInfo.suffix());

    int dupNum = engine.getDupNum(currentRow);

    if(dupNum > 1)
    {
        for(int i = 1; i < dupNum; i++)
        {
            ui->listWidgetDup->addItem(engine.getDupName(currentRow, i));
        }
        ui->listWidgetDup->setCurrentRow(0);
    }

    showImage(fileInfo.absoluteFilePath(), ui->graphicsViewOrig, origScene);
}

void MainWindow::on_listWidgetDup_currentRowChanged(int currentRow)
{
    selectedDupFileId = currentRow + 1;
    QFileInfo fileInfo = engine.getDubFileInfo(selectedFileId, selectedDupFileId);

    ui->labelDupPath->setText(fileInfo.absoluteFilePath());
    ui->labelDupChangeDate->setText(fileInfo.lastModified().toString());
    ui->labelDupSaveDate->setText(fileInfo.birthTime().toString());
    ui->labelDupSize->setText(QString::number(fileInfo.size()));
    ui->labelDupType->setText(fileInfo.suffix());
    ui->labelDupExt->setText(fileInfo.suffix());

    showImage(fileInfo.absoluteFilePath(), ui->graphicsViewDup, dupScene);
}

void MainWindow::on_pushButtonFullCompare_clicked()
{
    thread.start();
    emit runFull();
}

