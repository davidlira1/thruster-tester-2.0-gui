#pragma once

#include "SerialService.h"

#include <QByteArray>
#include <QMainWindow>
#include <QTimer>
#include <QVector>

class QButtonGroup;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void showError(const QString &message);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void setCurrentPage(int pageIndex);
    void applySidebarForTab(int pageIndex);
    void refreshDetectedPorts();
    void refreshPortCombo(const QVector<SerialService::Port> &ports);
    void updateConnectButton();
    void updateTestButtons();
    void onConnectButtonClicked();
    void onStartButtonClicked();
    void onStopButtonClicked();
    void onSerialDataReceived(const QByteArray &payload);
    void onSerialError(const QString &message);
    void hideErrorToast();
    void positionErrorToast();

    Ui::MainWindow *ui;
    QButtonGroup *tabButtonGroup;
    SerialService *serialService;
    QTimer *portRefreshTimer;
    QByteArray m_rxBuffer;
    bool m_testRunning = false;
};
