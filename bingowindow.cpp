#include "bingowindow.h"
#include "ui_bingowindow.h"
#include <QDebug>

BingoWindow::BingoWindow(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui(new Ui::BingoWindow)
{
    ui->setupUi(this);
}

BingoWindow::~BingoWindow()
{
    delete ui;
}

void BingoWindow::resizeEvent(QResizeEvent *e) 
{
    size = e->size();
    qDebug() << "resizeEvent" << e->size();
    emit sizeChanged();
}

void BingoWindow::closeEvent(QCloseEvent *e)
{
    e->ignore();
}
