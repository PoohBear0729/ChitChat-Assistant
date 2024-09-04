#include "mainwindow.h"
#include "dialog.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog d;
    if(d.exec()==QDialog::Accepted){
        MainWindow w;
        w.show();
        return a.exec();
    }
    return 0;
}
