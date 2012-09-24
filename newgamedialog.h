#ifndef NEWGAMEDIALOG_H
#define NEWGAMEDIALOG_H

#include <QDialog>

namespace Ui {
class NewGameDialog;
}

class Game;

class NewGameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewGameDialog(QWidget *parent = 0);
    ~NewGameDialog();

    static Game *getGame(QWidget *parent = 0,
                         bool save_game = true,
                         bool render_cards = true);

private slots:
    void on_choose_place_clicked();

private:
    Ui::NewGameDialog *ui;
};

#endif // NEWGAMEDIALOG_H
