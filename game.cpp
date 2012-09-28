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
#include <QEventLoop>
#include <QApplication>
#include <QMessageBox>
#include "qjson/src/serializer.h"
#include "qjson/src/parser.h"

#include <iostream>
using namespace std;

Game::Game()
{
}

Game::Game(const QString &save_game, QWidget *parent)
{
    if (!QFile::exists(save_game)) {
        QMessageBox::information(parent, "errer opening game file", "could not locate game file");
        return;
    }
    QFile f(save_game);
    if (!f.open(QFile::ReadOnly)) {
        QMessageBox::information(parent, "errer opening game file", "could not open game file");
        return;
    }
    
    QString contents = f.readAll();
    QJson::Parser parser;
    bool ok;
    
    QList<QVariant> cards = parser.parse(contents.toAscii(), &ok).toList();
    if (!ok) {
        QMessageBox::information(parent, "errer opening game file", "could not parse game file");
        return;
    }
    foreach(QVariant card, cards) {
        Card c;
        Q_ASSERT(card.canConvert<QVariantList>());
        int i = 0;
        foreach(QVariant number, card.toList()) {
            Q_ASSERT(number.canConvert<int>());
            bool ok;
            c.numbers[i%5][i/5] = number.toInt(&ok);
            Q_ASSERT(ok);
            i++;
        }
        this->cards.push_back(c);
        this->stencils.push_back(Stencil());
    }
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
            numbers.push_back(c.numbers[i%5][i/5]);
        }
        cards.push_back(numbers);
    }
    QJson::Serializer ser;
    QFile f(dir.absoluteFilePath(name+".bgo"));
    f.open(QFile::WriteOnly);
    f.write(ser.serialize(cards));
    f.close();
}

struct number_replacer {
    const Card &c1, &c2;
    number_replacer(const Card &c1, const Card &c2) : c1(c1), c2(c2) {

    }

    std::string operator()(boost::smatch what) {
        int idx = boost::lexical_cast<int>(what.str(1)) - 1;
        if (idx < 25) {
            return boost::lexical_cast<std::string>(c1.numbers[idx%5][idx/5]);
        }
        idx -= 25;
        return boost::lexical_cast<std::string>(c2.numbers[idx%5][idx/5]);
    }
};

struct id_replacer {
    std::string id1, id2;
    
    id_replacer(int id1, int id2) {
        this->id1 = boost::lexical_cast<std::string>(id1);
        this->id2 = boost::lexical_cast<std::string>(id2);
    }
    
    std::string operator()(boost::smatch what) {
        std::string num = what.str(1);
        return num == "1" ? id1 : id2;
    }
};


void Game::renderCards(const QDir &dir, const QString &name, QWidget *parent)
{
    QPrinter printer;
    printer.setPageSize(QPrinter::A4);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOrientation(QPrinter::Landscape);
    printer.setOutputFileName(dir.absoluteFilePath(name+".pdf"));
    QPainter painter(&printer);
    
    QFile svg_template("imgs/template.svg");
    bool opened = svg_template.open(QFile::ReadOnly);
    Q_ASSERT(opened);
    QString orig_template = svg_template.readAll();
    std::string _template = orig_template.toStdString();

    QProgressDialog dialog(parent);
    dialog.setMinimum(0);
    dialog.setMaximum(cards.size()/2);
    dialog.setLabelText("Rendering Bingo Cards");

    boost::regex number_re("%([0-9]+)");
    boost::regex id_re("#id(1|2)");

    for (int i = 0; i < cards.size()/2; i++) {
        qApp->processEvents();
        dialog.setValue(i);
        if (dialog.wasCanceled()) {
            break;
        }
        Card c1 = cards[i*2];
        Card c2 = cards[i*2+1];

        number_replacer fmter(c1, c2);
        std::string svg_src = boost::regex_replace(_template, number_re, fmter);
        id_replacer idter(i*2, i*2+1);
        svg_src = boost::regex_replace(svg_src, id_re, idter);

        QSvgRenderer renderer(QString::fromStdString(svg_src).toAscii());
        renderer.render(&painter);
        printer.newPage();
        
    }

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
        card.numbers[i%5][i/5] = new_number;
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

    if (isVertical()) win << "Vertical";
    if (isHorizontal()) win << "Horizontal";
    if (isDiagonal()) win << "Diagonal";
    if (isCross()) win << "Cross";
    if (isCircle()) win << "Circle";
    if (isSoFuckingClose()) win << "So Fucking Close";
    if (isBingo()) win << "Bingo";
    return win.join(", ");
}
