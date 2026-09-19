#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QAbstractItemView>
#include <QButtonGroup>
#include <QEvent>
#include <QIcon>
#include <QKeyEvent>
#include <QListView>
#include <QPalette>
#include <QPushButton>
#include <QStyle>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , tabButtonGroup(new QButtonGroup(this))
    , serialService(new SerialService(this))
    , portRefreshTimer(new QTimer(this))
{
    ui->setupUi(this);

    tabButtonGroup->setExclusive(true);
    tabButtonGroup->addButton(ui->testTabButton, 0);
    tabButtonGroup->addButton(ui->calibrationTabButton, 1);
    tabButtonGroup->addButton(ui->developerTabButton, 2);

    connect(tabButtonGroup, &QButtonGroup::idClicked,
            this, &MainWindow::setCurrentPage);

    auto *portListView = new QListView(ui->portCombo);
    portListView->setUniformItemSizes(true);
    ui->portCombo->setView(portListView);
    ui->portCombo->setMaxVisibleItems(8);
    ui->portCombo->installEventFilter(this);
    connect(ui->portCombo, &QComboBox::currentIndexChanged,
            this, &MainWindow::updateConnectButton);
    connect(ui->connectButton, &QPushButton::clicked,
            this, &MainWindow::onConnectButtonClicked);

    portRefreshTimer->setInterval(5000);
    connect(portRefreshTimer, &QTimer::timeout, this, [this]() {
        if (!serialService->isConnected() && !ui->portCombo->view()->isVisible()) {
            refreshDetectedPorts();
        }
    });

    setCurrentPage(0);
    refreshDetectedPorts();
    portRefreshTimer->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->portCombo && !ui->portCombo->view()->isVisible()) {
        const bool mouseOpen = event->type() == QEvent::MouseButtonPress;
        const bool keyOpen = event->type() == QEvent::KeyPress
            && (static_cast<QKeyEvent *>(event)->key() == Qt::Key_Down
                || static_cast<QKeyEvent *>(event)->key() == Qt::Key_F4);

        if ((mouseOpen || keyOpen) && !serialService->isConnected()) {
            refreshDetectedPorts();
        }
    }

    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::setCurrentPage(int pageIndex)
{
    ui->contentStack->setCurrentIndex(pageIndex);
    applySidebarForTab(pageIndex);
}

void MainWindow::applySidebarForTab(int pageIndex)
{
    ui->serialFrame->setVisible(true);
    ui->testFrame->setVisible(pageIndex == 0);
    ui->calibrationFrame->setVisible(pageIndex == 1);
    ui->developerFrame->setVisible(pageIndex == 2);
}

void MainWindow::refreshDetectedPorts()
{
    const QString selectedPort = ui->portCombo->currentData(Qt::UserRole).toString();
    refreshPortCombo(serialService->availablePorts());

    if (selectedPort.isEmpty()) {
        return;
    }

    for (int i = 0; i < ui->portCombo->count(); ++i) {
        if (ui->portCombo->itemData(i, Qt::UserRole).toString() == selectedPort) {
            ui->portCombo->setCurrentIndex(i);
            return;
        }
    }
}

void MainWindow::refreshPortCombo(const QVector<SerialService::Port> &ports)
{
    ui->portCombo->clear();

    QPalette palette = ui->portCombo->palette();
    palette.setColor(QPalette::PlaceholderText, QColor(QStringLiteral("#8aa0b0")));
    ui->portCombo->setPalette(palette);

    if (ports.isEmpty()) {
        ui->portCombo->setPlaceholderText(QStringLiteral("No Devices Detected"));
    } else {
        for (const SerialService::Port &port : ports) {
            ui->portCombo->addItem(port.displayText, port.portName);
        }
        const int portCount = ports.size();
        ui->portCombo->setPlaceholderText(
            portCount == 1
                ? QStringLiteral("Select (1 port available)")
                : QStringLiteral("Select (%1 ports available)").arg(portCount));
    }

    ui->portCombo->setCurrentIndex(-1);
}

void MainWindow::updateConnectButton()
{
    const bool connected = serialService->isConnected();
    const bool hasSelection = ui->portCombo->currentIndex() >= 0
        && !ui->portCombo->currentData(Qt::UserRole).toString().isEmpty();

    if (connected) {
        ui->connectButton->setText(QStringLiteral("Connected"));
        ui->connectButton->setIcon(QIcon(QStringLiteral(":/icons/status-dot.svg")));
        ui->connectButton->setIconSize(QSize(10, 10));
    } else if (hasSelection) {
        ui->connectButton->setText(QStringLiteral("Connect"));
        ui->connectButton->setIcon(QIcon());
    } else {
        ui->connectButton->setText(QStringLiteral("Disconnected"));
        ui->connectButton->setIcon(QIcon());
    }

    ui->portCombo->setEnabled(!connected);
    ui->connectButton->setProperty("connectable", hasSelection && !connected);
    ui->connectButton->setProperty("connected", connected);
    ui->connectButton->style()->unpolish(ui->connectButton);
    ui->connectButton->style()->polish(ui->connectButton);
}

void MainWindow::onConnectButtonClicked()
{
    if (serialService->isConnected()) {
        serialService->disconnectPort();
        updateConnectButton();
        return;
    }

    const QString portName = ui->portCombo->currentData(Qt::UserRole).toString();
    if (portName.isEmpty()) {
        return;
    }

    serialService->connectTo(portName);
    updateConnectButton();
}
