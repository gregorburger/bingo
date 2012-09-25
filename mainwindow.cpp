#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_bingowindow.h"

#include "bingowindow.h"
#include "newgamedialog.h"

#include <QDebug>
#include <QKeyEvent>
#include <QDeclarativeContext>
#include <QDeclarativeView>
#include <QVariant>
#include <QGraphicsObject>
#include <QSettings>
#include <QFileDialog>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), fullscreen(false), bingo(false), wedgie(false), countdown(false)
{
    ui->setupUi(this);
    bingoWindow = new BingoWindow(this);
    
    declarativeView = bingoWindow->ui->declarativeView;
    
    context = declarativeView->rootContext();
    context->setContextProperty("mainwindow", this);
    context->setContextProperty("bingowindow", bingoWindow);
   
    declarativeView->setSource(QUrl("qrc:/qml/mainview.qml"));
    addAction(ui->actionFullscreen);
    bingoWindow->addAction(ui->actionFullscreen);
    QObject::connect(&countDownTimer, SIGNAL(timeout()), this, SLOT(secondPassed()));

    QSettings settings;
    restoreGeometry(settings.value("mw-geometry").toByteArray());
    restoreState(settings.value("mw-state").toByteArray());
    bingoWindow->restoreGeometry(settings.value("bw-geometry").toByteArray());

    bingoWindow->show();
    this->connect(ui->numberList->verticalScrollBar(), SIGNAL(rangeChanged(int,int)), SLOT(ensure_Scrolled_Down(int,int)));
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
    _countDown = 60*20; //twenty minutes
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

        game.set(number);
        ui->possibleWinners->clear();
        ui->possibleWinners->addItems(game.getPossibleWinners());
    }
}

void MainWindow::on_numberList_itemDoubleClicked(QListWidgetItem *item)
{
    int row = ui->numberList->row(item);
    int number = ui->numberList->item(row)->text().toInt();
    delete ui->numberList->takeItem(row);
    numbers.removeAt(numbers.size()-row-1);
    emit oldNumbersChanged();
    emit lastNumberChanged();

    game.unset(number);
    ui->possibleWinners->clear();
    ui->possibleWinners->addItems(game.getPossibleWinners());
}

void MainWindow::closeEvent(QCloseEvent *event) {
    QSettings settings;
    settings.setValue("mw-geometry", saveGeometry());
    settings.setValue("mw-state", saveState());
    settings.setValue("bw-geometry", bingoWindow->saveGeometry());
    QMainWindow::closeEvent(event);
}

void MainWindow::on_actionNew_Game_triggered()
{
    Game *game = NewGameDialog::getGame(this);
    if (game) {
        this->game = *game;
        delete game;
    }
}

void MainWindow::on_actionOpen_Game_triggered()
{
    QString file = QFileDialog::getOpenFileName(this, "Open Game", QDir::currentPath(), "Bingo File (*.bgo)");
    if (file != "") {
        game = Game(file, this);
    }
}

void MainWindow::ensure_Scrolled_Down(int min, int max)
{
    Q_UNUSED(min);
    QScrollBar *sb = ui->numberList->verticalScrollBar();
    sb->setValue(max);
}

