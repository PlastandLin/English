#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow window;
    window.setWindowTitle("Full English Level Test");
    window.resize(500, 400);
    window.show();
    return app.exec();
}
