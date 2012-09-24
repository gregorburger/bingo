#include "game.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include <QSet>
#include <QVariantMap>
#include <QVariantList>
#include <QDebug>
#include <QPrinter>
#include <QPainter>
#include <QSvgRenderer>
#include <QTemporaryFile>
#include <QProgressDialog>
#include "qjson/src/serializer.h"

#include <iostream>
using namespace std;

Game::Game()
{
}

Game::Game(const QString &save_game)
{

}

Game::Game(int number_of_cards)
{
    for (int i = 0; i < number_of_cards; ++i) {
        cards.push_back(Card::random());
        stencils.push_back(Stencil());
    }
}

QStringList Game::getPossibleWinners() const
{
    QStringList winners;
    for (int i = 0; i < cards.size(); ++i) {
        QString win = stencils[i].getWin();
        if (win != "")
            winners << QString("%1 (%2)").arg(i).arg(win);
    }
    return winners;
}

void Game::saveGame(const QDir &dir, const QString &name)
{
    QVariantList cards;
    foreach(Card c, this->cards) {
        QVariantList numbers;
        for (int i = 0; i < 5*5; i++) {
            numbers.push_back(c.numbers[i/5][i%5]);
        }
        cards.push_back(numbers);
    }
    QJson::Serializer ser;
    QFile f(dir.absoluteFilePath(name+".bgo"));
    f.open(QFile::WriteOnly);
    f.write(ser.serialize(cards));
    f.close();
}

struct formatter {
    const Card &c1, &c2;
    formatter(const Card &c1, const Card &c2) : c1(c1), c2(c2) {

    }

    std::string operator()(boost::smatch what) {
        std::string num = what.str(0);
        std::string num1 = std::string(num.begin()+1, num.end());
        int idx = boost::lexical_cast<int>(num1) - 1;
        if (idx < 25) {
            return boost::lexical_cast<std::string>(c1.numbers[idx/5][idx%5]);
        }
        idx -= 25;
        return boost::lexical_cast<std::string>(c2.numbers[idx/5][idx%5]);
        return "gag";
    }
};


void Game::renderCards(const QDir &dir, const QString &name)
{
    QPrinter printer;
    printer.setPageSize(QPrinter::A4);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOrientation(QPrinter::Landscape);
    printer.setOutputFileName(dir.absoluteFilePath(name+".pdf"));
    QPainter painter(&printer);

    QFile svg_template;
    svg_template.setFileName(":/imgs/template.svg");
    bool opened = svg_template.open(QFile::ReadOnly);
    Q_ASSERT(opened);
    QString orig_template = svg_template.readAll();
    std::string _template = orig_template.toStdString();

    QProgressDialog dialog("rendering cards", "cancel", 0, cards.size()/2);

    boost::regex regexp("%([0-9]+)");

    for (int i = 0; i < cards.size()/2; i++) {
        Card c1 = cards[i*2];
        Card c2 = cards[i*2+1];

        formatter fmter(c1, c2);
        std::string svg_src = boost::regex_replace(_template, regexp, fmter);

        QTemporaryFile tmpf("XXXXXXXX.svg");
        tmpf.setAutoRemove(false);
        tmpf.open();
        QTextStream out(&tmpf);

        out << QString::fromStdString(svg_src);

        QSvgRenderer renderer(QString::fromStdString(svg_src).toAscii());
        renderer.render(&painter);
        printer.newPage();
        dialog.setValue(i);
    }

}

void Game::load_game(const QString &save_game)
{
}

boost::random::mt19937 rng(std::time(0));
boost::random::uniform_int_distribution<> gen(1, 75);

Card Card::random()
{
    Card card;
    int i = 0;
    QSet<int> card_numbers;
    while (i < 5*5) {
        int new_number = gen(rng);
        cout << new_number << " ";
        if (card_numbers.contains(new_number)) continue;
        card_numbers.insert(new_number);
        card.numbers[i/5][i%5] = new_number;
        i++;
    }
    cout << endl;
    return card;
}

Stencil::Stencil() {
    for (size_t x = 0; x < numbers.size(); ++x) {
        for (size_t y = 0; y < numbers[x].size(); ++y) {
            numbers[x][y] = false;
        }
    }
    numbers[2][2] = true; //middle is a win
}

void Game::set_unset(int number, bool v)
{
    for (int i = 0; i < cards.size(); ++i) {
        for (size_t x = 0; x < 5; ++x) {
            for (size_t y = 0; y < 5; ++y) {
                if (x == 2 && y == 2) continue;
                if (cards[i].numbers[x][y] == number) {
                    stencils[i].numbers[x][y] = v;
                }
            }
        }
    }
}


bool Stencil::isStar() const
{
    bool win = true;
    for (int x = 0; x < 5; ++x) {
        for (int y = 0; y < 5; ++y) {
            if (x == 1 && y == 0) continue;
            if (x == 0 && y == 1) continue;
            if (x == 3 && y == 0) continue;
            if (x == 0 && y == 3) continue;
            if (x == 4 && y == 1) continue;
            if (x == 1 && y == 4) continue;
            if (x == 4 && y == 3) continue;
            if (x == 3 && y == 4) continue;

            win = win && numbers[x][y];
        }
    }
    return win;
}

bool Stencil::isVertical() const
{
    bool win = false;
    for (int x = 0; x < 5; ++x) {
        bool line = true;
        for (int y = 0; y < 5; ++y) {
            line = line && numbers[x][y];
        }
        win = win || line;
    }
    return win;
}

bool Stencil::isHorizontal() const
{
    bool win = false;
    for (int y = 0; y < 5; ++y) {
        bool row = true;
        for (int x = 0; x < 5; ++x) {
            row = row && numbers[x][y];
        }
        win = win || row;
    }
    return win;
}

bool Stencil::isDiagonal() const
{
    bool d1 = true;
    bool d2 = true;
    for (int i = 0; i < 5; ++i) {
        d1 = d1 && numbers[i][i];
        d2 = d2 && numbers[i][4-i];
    }
    return d1 || d2;
}

bool Stencil::isCross() const
{
    bool win = true;
    for (int i = 0; i < 5; ++i) {
        win = win && numbers[i][2];
        win = win && numbers[2][i];
    }
    return win;
}

bool Stencil::isPenis() const
{
    bool win = true;
    for (int i = 0; i < 5; ++i) {
        win = win && numbers[1][i];
        win = win && numbers[3][i];
    }
    win = win && numbers[2][0];

    win = win && numbers[0][3];
    win = win && numbers[0][4];

    win = win && numbers[4][3];
    win = win && numbers[4][4];

    return win;
}


bool Stencil::isCircle() const
{
    bool win = true;
    for (int i = 0; i < 5; ++i) {
        win = win && numbers[i][0];
        win = win && numbers[i][4];
        win = win && numbers[0][i];
        win = win && numbers[4][i];
    }
    return win;
}

bool Stencil::isSoFuckingClose() const
{
    bool win = true;
    for (int x = 0; x < 5; ++x) {
        for (int y = 0; y < 5; ++y) {
            if (x == 4 && y == 4) {
                continue;
            }
            win = win && numbers[x][y];
        }
    }
    return win;
}

bool Stencil::isBingo() const
{
    bool win = true;
    for (int x = 0; x < 5; ++x) {
        for (int y = 0; y < 5; ++y) {
            win = win && numbers[x][y];
        }
    }
    return win;
}


QString Stencil::getWin() const
{
    QStringList win;

    if (isStar()) win << "Star";
    if (isVertical()) win << "Vertical";
    if (isHorizontal()) win << "Horizontal";
    if (isDiagonal()) win << "Diagonal";
    if (isCross()) win << "Cross";
    if (isPenis()) win << "Penis";
    if (isCircle()) win << "Circle";
    if (isSoFuckingClose()) win << "So Fucking Close";
    if (isBingo()) win << "Bingo";
    return win.join(", ");
}
