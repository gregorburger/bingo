#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
   QApplication a(argc, argv);
   Q_INIT_RESOURCE(bingo);
   QCoreApplication::setApplicationName(QString::fromAscii("bingo"));
   QCoreApplication::setOrganizationName(QString::fromAscii("ripbmxibk"));
   QCoreApplication::setOrganizationDomain(QString::fromAscii("ripbmxibk.com"));
   MainWindow w;
   w.show();
   
   return a.exec();
}
