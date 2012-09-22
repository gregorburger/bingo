#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_bingowindow.h"

#include "bingowindow.h"

#include <QDebug>
#include <QKeyEvent>
#include <QDeclarativeContext>
#include <QDeclarativeView>
#include <QVariant>
#include <QGraphicsObject>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), fullscreen(false), bingo(false), wedgie(false), countdown(false)
{
    ui->setupUi(this);
    bingoWindow = new BingoWindow(this);
    bingoWindow->show();
    
    declarativeView = bingoWindow->ui->declarativeView;
    
    context = declarativeView->rootContext();
    context->setContextProperty("mainwindow", this);
    context->setContextProperty("bingowindow", bingoWindow);
   
    declarativeView->setSource(QUrl("qrc:/qml/mainview.qml"));
    addAction(ui->actionFullscreen);
    bingoWindow->addAction(ui->actionFullscreen);
    QObject::connect(&countDownTimer, SIGNAL(timeout()), this, SLOT(secondPassed()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionFullscreen_triggered() 
{
    fullscreen = !fullscreen;
    if (fullscreen) {
        bingoWindow->showFullScreen();
    } else {
        bingoWindow->showNormal();
    }
}

QStringList MainWindow::oldNumbers() const 
{
    if (numbers.empty()) {
        return numbers;
    } else {
        return numbers.mid(1);
    }
}


void MainWindow::startCountDown() {
    _countDown = 60*10; //ten minutes
    declarativeView->setSource(QUrl("qrc:/qml/Countdown.qml"));

    countDownTimer.start(1000);
    countDownTimer.setSingleShot(false);
}

void MainWindow::secondPassed() {
    if (_countDown <= 0) {
        countDownTimer.stop();
        declarativeView->setSource(QUrl("qrc:/qml/mainview.qml"));
    }
    _countDown--;
    emit countDownChanged();
}

#include <cardgenerator.h>

void MainWindow::on_actionGenerate_Cards_triggered() {
    CardGenerator g;
    g.generate("Untitled", 24);
}

void MainWindow::on_bingoButton_toggled(bool checked)
{
    bingo = checked;
    img = "qrc:/imgs/imgs/bingo.png";
    emit bingoChanged();
    emit backImageChanged();
}

void MainWindow::on_countdownButton_toggled(bool checked)
{
    countdown = checked;
    emit countdownChanged();
    if (checked) {
        startCountDown();
    } else {
        declarativeView->setSource(QUrl("qrc:/qml/mainview.qml"));
    }
}

void MainWindow::on_wedgieButton_toggled(bool checked)
{
    wedgie = checked;
    img = "qrc:/imgs/imgs/wedgie.png";
    emit wedgieChanged();
    emit backImageChanged();
}

void MainWindow::on_lineEdit_returnPressed()
{
    bool ok;
    int number = ui->lineEdit->text().toInt(&ok);
    if (number <= 75 && ok) {
        ui->numberList->addItem(ui->lineEdit->text());
        numbers.push_front(ui->lineEdit->text());
        emit lastNumberChanged();
        emit oldNumbersChanged();
        ui->lineEdit->clear();
    }
}

void MainWindow::on_numberList_itemDoubleClicked(QListWidgetItem *item)
{
    int row = ui->numberList->row(item);
    delete ui->numberList->takeItem(row);
    numbers.removeAt(numbers.size()-row-1);
    emit oldNumbersChanged();
    emit lastNumberChanged();
}
