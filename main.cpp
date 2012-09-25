#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
   QApplication a(argc, argv);
   Q_INIT_RESOURCE(bingo);
   QCoreApplication::setApplicationName("bingo");
   QCoreApplication::setOrganizationName("ripbmxibk");
   QCoreApplication::setOrganizationDomain("ripbmxibk.com");
   MainWindow w;
   w.show();
   
   return a.exec();
}
