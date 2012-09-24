#ifndef GAME_H
#define GAME_H

#include <QString>
#include <QStringList>
#include <QDir>
#include <QVector>

#include <boost/array.hpp>

struct Card {
    boost::array<boost::array<int, 5>, 5> numbers;
    static Card random();
};

struct Stencil {
    Stencil();
    boost::array<boost::array<bool, 5>, 5> numbers;
    void set(int number);
    QString getWin() const;
    bool isStar() const;
    bool isVertical() const;
    bool isHorizontal() const;
    bool isDiagonal() const;
    bool isCross() const;
    bool isPenis() const;
    bool isCircle() const;
    bool isSoFuckingClose() const;
    bool isBingo() const;
};

class Game
{
public:
    Game();
    Game(const QString &save_game, QWidget *parent);
    Game(int number_of_cards);

    QStringList getPossibleWinners() const;
    void saveGame(const QDir &dir, const QString &name);
    void renderCards(const QDir &dir, const QString &name, QWidget *parent);

    void set(int number) {
        set_unset(number, true);
    }

    void unset(int number) {
        set_unset(number, false);
    }

private:
    void set_unset(int number, bool v);
    void load_game(const QString &save_game);
    QVector<Card> cards;
    QVector<Stencil> stencils;
};

#endif // GAME_H
