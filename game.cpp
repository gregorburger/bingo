#include "game.h"

#include <random>
#include <regex>
#include <string>
#include <ctime>

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
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <iostream>
using namespace std;

Game::Game() {
}

Game::Game(const QString &save_game, QWidget *parent) {
    if (!QFile::exists(save_game)) {
        QMessageBox::information(parent, "errer opening game file", "could not locate game file");
        return;
    }
    QFile f(save_game);
    if (!f.open(QFile::ReadOnly)) {
        QMessageBox::information(parent, "errer opening game file", "could not open game file");
        return;
    }
    
    QByteArray contents = f.readAll();
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(contents, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        QMessageBox::information(parent, "error opening game file", "could not parse game file: " + parseError.errorString());
        return;
    }

    QJsonObject bingo = doc.object();

    this->max_numbers = bingo["max_numbers"].toInt();

    QJsonArray cards = bingo["cards"].toArray();

    for (const QJsonValue &cardValue : cards) {
        Card c;
        QJsonArray cardNumbers = cardValue.toArray();
        int i = 0;
        for (const QJsonValue &numberValue : cardNumbers) {
            c.numbers[i%5][i/5] = numberValue.toInt();
            i++;
        }
        this->cards.push_back(c);
        this->stencils.push_back(Stencil());
    }
}

Game::Game(int number_of_cards, int max_numbers) : max_numbers(max_numbers) {
    for (int i = 0; i < number_of_cards; ++i) {
        cards.push_back(Card::random(max_numbers));
        stencils.push_back(Stencil());
    }
}

QStringList Game::getPossibleWinners() const {
    QStringList winners;
    for (int i = 0; i < cards.size(); ++i) {
        QString win = stencils[i].getWin();
        if (win != "")
            winners << QString("%1 (%2)").arg(i).arg(win);
    }
    return winners;
}

void Game::saveGame(const QDir &dir, const QString &name) {
    QJsonObject bingo;

    bingo["max_numbers"] = max_numbers;

    QJsonArray cards;
    for (const Card &c : this->cards) {
        QJsonArray numbers;
        for (int i = 0; i < 5*5; i++) {
            numbers.append(c.numbers[i%5][i/5]);
        }
        cards.append(numbers);
    }

    bingo["cards"] = cards;

    QJsonDocument doc(bingo);
    QFile f(dir.absoluteFilePath(name+".bgo"));
    f.open(QFile::WriteOnly);
    f.write(doc.toJson());
    f.close();
}

std::string number_replacer(const std::smatch& what, const Card &c1, const Card &c2) {
    int idx = std::stoi(what.str(1)) - 1;
    if (idx < 25) {
        return std::to_string(c1.numbers[idx%5][idx/5]);
    }
    idx -= 25;
    return std::to_string(c2.numbers[idx%5][idx/5]);
}

std::string id_replacer(const std::smatch& what, int id1, int id2) {
    std::string num = what.str(1);
    return num == "1" ? std::to_string(id1) : std::to_string(id2);
}


void Game::renderCards(const QDir &dir, const QString &name, QWidget *parent) {
    QPrinter printer;
    printer.setPageSize(QPageSize::A4);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageOrientation(QPageLayout::Landscape);
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

    std::regex number_re("%([0-9]+)");
    std::regex id_re("#id(1|2)");

    for (int i = 0; i < cards.size()/2; i++) {
        qApp->processEvents();
        dialog.setValue(i);
        if (dialog.wasCanceled()) {
            break;
        }
        Card c1 = cards[i*2];
        Card c2 = cards[i*2+1];

        // Replace numbers manually
        std::string svg_src = _template;
        std::string result;
        auto numbers_begin = std::sregex_iterator(svg_src.begin(), svg_src.end(), number_re);
        auto numbers_end = std::sregex_iterator();
        size_t last_pos = 0;

        for (std::sregex_iterator it = numbers_begin; it != numbers_end; ++it) {
            std::smatch match = *it;
            result.append(svg_src, last_pos, match.position() - last_pos);
            result.append(number_replacer(match, c1, c2));
            last_pos = match.position() + match.length();
        }
        result.append(svg_src, last_pos, std::string::npos);
        svg_src = result;

        // Replace IDs manually
        result.clear();
        auto ids_begin = std::sregex_iterator(svg_src.begin(), svg_src.end(), id_re);
        auto ids_end = std::sregex_iterator();
        last_pos = 0;

        for (std::sregex_iterator it = ids_begin; it != ids_end; ++it) {
            std::smatch match = *it;
            result.append(svg_src, last_pos, match.position() - last_pos);
            result.append(id_replacer(match, i*2, i*2+1));
            last_pos = match.position() + match.length();
        }
        result.append(svg_src, last_pos, std::string::npos);

        QSvgRenderer renderer(QString::fromStdString(result).toLatin1());
        renderer.render(&painter);
        printer.newPage();

    }

}

std::mt19937 rng(std::time(0));


Card Card::random(int max_numbers) {
    std::uniform_int_distribution<> gen(1, max_numbers);
    Card card;
    int i = 0;
    QSet<int> card_numbers;
    while (i < 5*5) {
        int new_number = gen(rng);
        //cout << new_number << " ";
        if (card_numbers.contains(new_number)) continue;
        card_numbers.insert(new_number);
        card.numbers[i%5][i/5] = new_number;
        i++;
    }
    //cout << endl;
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

void Game::set_unset(int number, bool v) {
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


bool Stencil::isStar() const {
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

bool Stencil::isVertical() const {
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

bool Stencil::isHorizontal() const {
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

bool Stencil::isDiagonal() const {
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
