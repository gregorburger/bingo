#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class QDeclarativeView;
class QDeclarativeContext;
class QListWidgetItem;
class BingoWindow;

class MainWindow : public QMainWindow
{
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
        return numbers.size() > 0 ? numbers.first() : "!";
    }
    
    QStringList oldNumbers() const;

    QString countDown() const {
        return QString("%1:%2").arg(_countDown/60).arg(_countDown%60);
    }

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
        if (isBingo()) {
            return "qrc:/imgs/imgs/bingo.png";
        } else {
            return "qrc:/imgs/imgs/wedgie.jpg";
        }
    }

private slots:
    void on_actionFullscreen_triggered();
    void secondPassed();
    
    void on_actionGenerate_Cards_triggered();
    
    void on_bingoButton_toggled(bool checked);
    
    void on_countdownButton_toggled(bool checked);
    
    void on_wedgieButton_toggled(bool checked);
    
    void on_lineEdit_returnPressed();
    
    void on_numberList_itemDoubleClicked(QListWidgetItem *item);
    
private:
    
    Ui::MainWindow *ui;
    BingoWindow *bingoWindow;
    bool fullscreen;
    bool bingo, wedgie, countdown;
    
    QStringList numbers;
    QDeclarativeView *declarativeView;
    QDeclarativeContext *context;
    
    int _countDown;
    QTimer countDownTimer;
    
};

#endif // MAINWINDOW_H
