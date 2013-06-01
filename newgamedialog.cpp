#include "newgamedialog.h"
#include "ui_newgamedialog.h"
#include "game.h"

#include <QFileDialog>
#include <QCompleter>
#include <QDirModel>

NewGameDialog::NewGameDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewGameDialog) {
    ui->setupUi(this);
    QDirModel *model = new QDirModel(this);
    QCompleter *completer = new QCompleter(model, this);
    ui->place->setCompleter(completer);
}

NewGameDialog::~NewGameDialog() {
    delete ui;
}

Game *NewGameDialog::getGame(QWidget *parent, bool save_game, bool render_cards) {
    NewGameDialog dialog(parent);
    if (dialog.exec() == QDialog::Accepted) {
        Game *g = new Game(dialog.ui->count->value(), dialog.ui->max_numbers->value());
        if (save_game)
            g->saveGame(QDir(dialog.ui->place->text()), dialog.ui->name->text());
        if (render_cards)
            g->renderCards(QDir(dialog.ui->place->text()), dialog.ui->name->text(), &dialog);
        return g;
    }
    return 0;
}

void NewGameDialog::on_choose_place_clicked() {
    QString path = QFileDialog::getExistingDirectory(this, "Choose Place to store the Game", QDir::currentPath());
    if (path != "")
        ui->place->setText(path);
}
