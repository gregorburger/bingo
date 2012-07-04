#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QKeyEvent>
#include <QDeclarativeContext>
#include <QVariant>
#include <QGraphicsObject>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), fullscreen(false)
{
    ui->setupUi(this);
    context = ui->declarativeView->rootContext();
    context->setContextProperty("mainwindow", this);
    
    ui->declarativeView->setSource(QUrl("mainview.qml"));
    addAction(ui->actionFullscreen);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionFullscreen_triggered()
{
    fullscreen = !fullscreen;
    if (fullscreen) {
        this->showFullScreen();
        this->menuBar()->setVisible(false);
        ui->actionFullscreen->setEnabled(true);
    } else {
        this->showNormal();
        this->menuBar()->setVisible(true);
    }
}

void MainWindow::new_Number()
{
    qDebug() << "new Bingo number " << input;
    int last_number = input.toInt();
    Q_ASSERT(last_number <= 75);
    numbers.push_front(input);
    emit lastNumberChanged();
    emit oldNumbersChanged();
    input = "";
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if (e->key() >= Qt::Key_0 && e->key() <= Qt::Key_9) {
        if (input.length() && input[0]=='7' && e->key() > Qt::Key_5) {
            qDebug() << "second number after 7 must be smaller than 6";
            return;
        }
        if (!input.length() && e->key() > Qt::Key_7) {
            qDebug() << "first number must be smaller than 8";
            return;
        }
        input += e->key();
        if (input.length() >= 2) {
            new_Number();
        }
    }
    if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) {
        qDebug() << "enter";
    }
    if (e->key() == Qt::Key_Escape || e->key() == Qt::Key_Backspace) {
        qDebug() << "back";
    }
    if (e->key() == Qt::Key_B) {
        qDebug() << "BINGO!!!";
    }
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    size = e->size();
    qDebug() << "resizeEvent" << e->size();
    emit sizeChanged();
}



QString MainWindow::lastNumber() const {
    return numbers.empty() ? QString("!") : numbers[0];
}

QStringList MainWindow::oldNumbers() const {
    if (numbers.empty()) {
        return numbers;
    } else {
        return numbers.mid(1);
    }
}
