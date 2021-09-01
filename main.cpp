#include <QApplication>

#include "Control/controle.h"
#include "View/mainwindow.h"

int main(int argc, char *argv[]) {

    Q_INIT_RESOURCE(mdi);

    QApplication app(argc, argv);

    (new Controle( new MainWindow() ))->iniciarAplicacao();

    return app.exec();

}
