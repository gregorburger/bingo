#include "cardgenerator.h"
#include <QPrinter>
#include <QPainter>
#include <QDebug>
#include <QSet>

#include <boost/random.hpp>

void CardGenerator::generate(const QString &name, int amount, const QDir &dest) {
    QPrinter printer;
    
    printer.setPageSize(QPrinter::A4);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(dest.absoluteFilePath(name + ".pdf"));
    
    Card *cards = generateCards(amount);
    
    //QRectF paperSize = 
   
    QPainter p(&printer);
    p.setWindow(0, 0, 
                printer.paperSize(QPrinter::Millimeter).width(), 
                printer.paperSize(QPrinter::Millimeter).height());
    
    for (int i = 0; i < amount; i++) {
        if (i % 6 == 0 && i > 0) {
            qDebug() << "new page" << i;
            printer.newPage();
        }
        
        int position = i % 6; //six per page
        
        
        int row = position % 2;
        int col = position / 2;
        
        QPen pen = p.pen();
        pen.setWidth(1);
        p.setPen(pen);
        
        p.save();
        p.translate(30+row*75, 13+col*90);
        
        //draw the border
        //p.drawRect(0, 0, 75, 90);
        
        //draw top decoration
        
        QRect top_box(0, 0, 75, 15);
        QFont f = p.font();
        f.setFamily("Arial Black");
        f.setPointSize(7);
        f.setLetterSpacing(QFont::PercentageSpacing, 90);
        //qDebug() << f.letterSpacing() << " " << f.letterSpacingType();
        p.setFont(f);
        p.drawRect(top_box);
        p.drawText(top_box, Qt::AlignCenter, "BOOZE BINGO");
        
        
        p.save();
        p.translate(0, 15);
        
        Card &card = cards[i];
        
        for (int j = 0; j < 5*5; j++) {
            int j_row = j % 5;
            int j_col = j / 5;
            QRect number_rect(j_row*15, j_col*15, 15, 15);
            QString number = QString("%1").arg(card.numbers[j]);
            p.drawText(number_rect, Qt::AlignCenter, number);
            p.drawRect(number_rect);
        }
        
        p.restore();
        
        
        p.restore();
    }
    delete[] cards;
}

Card *CardGenerator::generateCards(int amount) {
    Card *cards = new Card[amount];
    
    for (int i = 0; i < amount; ++i) {
        cards[i] = Card::random();
    }
    
    return cards;
}

#include <iostream>
#include <ctime>
using namespace std;

boost::random::mt19937 rng(std::time(0));
boost::random::uniform_int_distribution<> gen(1, 75);

Card Card::random() {
    Card card;
    int i = 0;
    QSet<int> card_numbers;
    while (i < 5*5) {
        int new_number = gen(rng);
        cout << new_number << " ";
        if (card_numbers.contains(new_number)) continue;
        card_numbers.insert(new_number);
        card.numbers[i] = new_number;
        i++;
    }
    cout << endl;
    return card;
}
