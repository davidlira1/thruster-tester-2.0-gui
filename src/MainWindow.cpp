#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QButtonGroup>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , tabButtonGroup(new QButtonGroup(this))
{
    ui->setupUi(this);

    tabButtonGroup->setExclusive(true);
    tabButtonGroup->addButton(ui->testTabButton, 0);
    tabButtonGroup->addButton(ui->calibrationTabButton, 1);
    tabButtonGroup->addButton(ui->developerTabButton, 2);

    connect(tabButtonGroup, &QButtonGroup::idClicked,
            this, &MainWindow::setCurrentPage);

    setCurrentPage(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setCurrentPage(int pageIndex)
{
    ui->contentStack->setCurrentIndex(pageIndex);
}
