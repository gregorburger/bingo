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

    foreach(QObject *child, ui->button_field->children()) {
        if (child->metaObject()->className() == QString("QPushButton")) {
            QPushButton *button = (QPushButton *) child;
            bool ok;
            int idx = button->text().toInt(&ok) - 1;
            if (!ok) continue;
            Q_ASSERT(idx >= 0 && idx < 75);
            buttons[idx] = button;
            state[idx] = false;
            this->connect(button, SIGNAL(toggled(bool)), SLOT(buttonToggled(bool)));
        }
    }

#if 0
    QActionGroup *group = new QActionGroup(ui->toolBar);
    group->addAction(ui->actionBingo);
    group->addAction(ui->actionCountdown);
    group->addAction(ui->actionWedgie);
#endif
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_actionFullscreen_triggered() {
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

void MainWindow::setMaxNumbers(int max_numbers) {
    foreach (QPushButton *button, buttons) {
        button->setEnabled(true);
    }

    for (int i = max_numbers; i < buttons.size(); i++) {
        buttons[i]->setEnabled(false);
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

void MainWindow::closeEvent(QCloseEvent *event) {
    QSettings settings;
    settings.setValue("mw-geometry", saveGeometry());
    settings.setValue("mw-state", saveState());
    settings.setValue("bw-geometry", bingoWindow->saveGeometry());
    QMainWindow::closeEvent(event);
}

void MainWindow::on_actionNew_Game_triggered() {
    Game *game = NewGameDialog::getGame(this);
    if (game) {
        this->game = *game;
        delete game;
    }
}

void MainWindow::on_actionOpen_Game_triggered() {
    QString file = QFileDialog::getOpenFileName(this, "Open Game", QDir::currentPath(), "Bingo File (*.bgo)");
    if (file != "") {
        game = Game(file, this);
        this->setMaxNumbers(game.max_numbers);
    }
}

void MainWindow::buttonToggled(bool t) {
    for (int i = 0; i < buttons.size(); ++i) {
        if (buttons[i]->isChecked() == state[i]) {
            continue;
        }
        state[i] = t;
        int number = i + 1;

        if (t) {
            numbers.push_front(QString("%1").arg(number));
            emit lastNumberChanged();
            emit oldNumbersChanged();

            game.set(number);
            ui->possibleWinners->clear();
            ui->possibleWinners->addItems(game.getPossibleWinners());
        } else {
            numbers.removeOne(QString("%1").arg(number));
            emit oldNumbersChanged();
            emit lastNumberChanged();

            game.unset(number);
            ui->possibleWinners->clear();
            ui->possibleWinners->addItems(game.getPossibleWinners());
        }
    }
}


void MainWindow::on_actionBingo_toggled(bool checked) {
    bingo = checked;
    img = "qrc:/imgs/imgs/bingo.png";
    emit bingoChanged();
    emit backImageChanged();
}

void MainWindow::on_actionCountdown_toggled(bool checked) {
    countdown = checked;
    emit countdownChanged();
    if (checked) {
        startCountDown();
    } else {
        declarativeView->setSource(QUrl("qrc:/qml/mainview.qml"));
    }
}

void MainWindow::on_actionWedgie_toggled(bool checked) {
    wedgie = checked;
    img = "qrc:/imgs/imgs/wedgie.png";
    emit wedgieChanged();
    emit backImageChanged();
}

