#ifndef BINGOWINDOW_H
#define BINGOWINDOW_H

#include <QDialog>
#include <QResizeEvent>

namespace Ui {
class BingoWindow;
}


class BingoWindow : public QWidget
{
    Q_OBJECT
    
    Q_PROPERTY(int width_
               READ width_()
               NOTIFY sizeChanged())
    
    Q_PROPERTY(int height_
               READ height_()
               NOTIFY sizeChanged())
    
public:
    explicit BingoWindow(QWidget *parent = 0);
    ~BingoWindow();
    
    int width_() const {
        return size.width();
    }
    
    int height_() const {
        return size.height();
    }
    
    void resizeEvent(QResizeEvent *e);
    
    Ui::BingoWindow *ui;
    
public slots:
    void closeEvent(QCloseEvent *);
    
signals:
    void sizeChanged();
    
private:
    QSize size;
};

#endif // BINGOWINDOW_H
