#include "MainWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    application.setApplicationName(QStringLiteral("Thruster Tester 2.0"));
    application.setOrganizationName(QStringLiteral("LBCC ROV"));

    MainWindow window;
    window.show();

    return application.exec();
}
