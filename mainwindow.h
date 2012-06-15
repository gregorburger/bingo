#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDeclarativeContext>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(QString last_number
               READ lastNumber()
               NOTIFY lastNumberChanged())
    
    Q_PROPERTY(QStringList old_numbers
               READ oldNumbers()
               NOTIFY oldNumbersChanged())
    
    Q_PROPERTY(int width_
               READ width_()
               NOTIFY sizeChanged())
    
    Q_PROPERTY(int height_
               READ height_()
               NOTIFY sizeChanged())
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
    QString lastNumber() const;
    
    QStringList oldNumbers() const;
    
    int width_() const {
        return size.width();
    }
    
    int height_() const {
        return size.height();
    }

signals:
    void lastNumberChanged();
    void oldNumbersChanged();
    void sizeChanged();
    
private slots:
    void on_actionFullscreen_triggered();
    void new_Number();
    
private:
    void keyPressEvent(QKeyEvent *);
    void resizeEvent(QResizeEvent *);
    
    Ui::MainWindow *ui;
    bool fullscreen;
    QString input;
    QStringList numbers;
    QDeclarativeContext *context;
    QSize size;
};

#endif // MAINWINDOW_H
