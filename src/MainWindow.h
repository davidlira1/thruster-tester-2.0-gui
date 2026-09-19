#pragma once

#include <QMainWindow>

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

private:
    void setCurrentPage(int pageIndex);

    Ui::MainWindow *ui;
    QButtonGroup *tabButtonGroup;
};
