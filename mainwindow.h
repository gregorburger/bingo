#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <boost/array.hpp>

#include "game.h"

namespace Ui {
class MainWindow;
}

class QDeclarativeView;
class QDeclarativeContext;
class QListWidgetItem;
class BingoWindow;
class QPushButton;

class MainWindow : public QMainWindow {
    Q_OBJECT
    Q_PROPERTY(QString last_number
               READ lastNumber()
               NOTIFY lastNumberChanged())
    
    Q_PROPERTY(QStringList old_numbers
               READ oldNumbers()
               NOTIFY oldNumbersChanged())

    Q_PROPERTY(QString countDown
               READ countDown()
               NOTIFY countDownChanged())
    
    Q_PROPERTY(bool countdown
               READ isCountdown()
               NOTIFY countdownChanged())
    
    Q_PROPERTY(bool bingo
               READ isBingo()
               NOTIFY bingoChanged())
    
    Q_PROPERTY(bool wedgie
               READ isWedgie()
               NOTIFY wedgieChanged())
    
    
    Q_PROPERTY(QString back_image
               READ backImage()
               NOTIFY backImageChanged())
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
    QString lastNumber() const {
        return numbers.size() > 0 ? numbers.first() : QString("!");
    }
    
    QStringList oldNumbers() const;

    QString countDown() const {
        return QString("%1:%2")
                .arg(_countDown/60, 2, 10, QLatin1Char('0'))
                .arg(_countDown%60, 2, 10, QLatin1Char('0'));
    }

    void setMaxNumbers(int max_numbers);

signals:
    void lastNumberChanged();
    void oldNumbersChanged();
    void countDownChanged();
    
    void countdownChanged();
    void bingoChanged();
    void wedgieChanged();
    void backImageChanged();
    

public slots:
    void startCountDown();
    
    bool isCountdown() const {
        return countdown;
    }
    
    bool isBingo() const {
        return bingo;
    }
    
    bool isWedgie() const {
        return wedgie;
    }
    
    QString backImage() const {
        return img;
    }


    void buttonToggled(bool t);

private slots:
    void secondPassed();

    void on_actionNew_Game_triggered();
    void on_actionOpen_Game_triggered();
    void on_actionFullscreen_triggered();

    void on_actionBingo_toggled(bool arg1);
    void on_actionCountdown_toggled(bool arg1);
    void on_actionWedgie_toggled(bool arg1);

private:
    void closeEvent(QCloseEvent *);
    
    Ui::MainWindow *ui;
    BingoWindow *bingoWindow;
    bool fullscreen;
    bool bingo, wedgie, countdown;
    
    QStringList numbers;
    QDeclarativeView *declarativeView;
    QDeclarativeContext *context;
    
    int _countDown;
    QTimer countDownTimer;
    QString img;

    Game game;

    boost::array<QPushButton *, 75> buttons;
    boost::array<bool, 75> state;
};

#endif // MAINWINDOW_H
