#ifndef CARDGENERATOR_H
#define CARDGENERATOR_H

#include <QDir>
#include <QList>

struct Card {
    int numbers[5*5];
    static Card random();
};

class CardGenerator {
public:
    void generate(const QString &name, 
                  int amount, 
                  const QDir &dest = QDir::current());
private:
    Card *generateCards(int amount);
};

#endif // CARDGENERATOR_H
